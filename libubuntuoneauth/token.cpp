/*
 * Copyright 2013-2015 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "token.h"
#include "ssoservice.h"

#include <stdlib.h>
#include <oauth.h>

#include <QCoreApplication>
#include <QHash>
#include <QHostInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>

#define TOKEN_NAME_KEY "name"
#define TOKEN_TOKEN_KEY "token"
#define TOKEN_TOKEN_SEC_KEY "token_secret"
#define TOKEN_CONSUMER_KEY "consumer_key"
#define TOKEN_CONSUMER_SEC_KEY "consumer_secret"
#define TOKEN_UPDATED_KEY "updated"
#define TOKEN_CREATED_KEY "created"

#define TIMESTAMP_CHECK_INTERVAL (60 * 60) // Seconds/minute * minutes


namespace UbuntuOne {

    Token::Token(QString token_key, QString token_secret,
                 QString consumer_key, QString consumer_secret)
    {
        _tokenHash[TOKEN_NAME_KEY] = buildTokenName();
        if (!token_key.isEmpty()) {
            _tokenHash[TOKEN_TOKEN_KEY] = token_key;
        }
        if (!token_secret.isEmpty()) {
            _tokenHash[TOKEN_TOKEN_SEC_KEY] = token_secret;
        }
        if (!consumer_key.isEmpty()) {
            _tokenHash[TOKEN_CONSUMER_KEY] = consumer_key;
        }
        if (!consumer_secret.isEmpty()) {
            _tokenHash[TOKEN_CONSUMER_SEC_KEY] = consumer_secret;
        }
    }

    Token::Token(QString token_key, QString token_secret,
                 QString consumer_key, QString consumer_secret,
                 QString created_date, QString updated_date):
        Token(token_key, token_secret, consumer_key, consumer_secret)
    {
        _tokenHash[TOKEN_CREATED_KEY] = dateStringToISO(created_date);
        _tokenHash[TOKEN_UPDATED_KEY] = dateStringToISO(updated_date);
    }

    /**
     * \fn QString Token::toQuery()
     *
     * Turn a Token into a query string.
     **/
    QString Token::toQuery()
    {
        QStringList secret;

        secret.prepend(QStringLiteral(TOKEN_NAME_KEY) + "=" + QUrl::toPercentEncoding(_tokenHash[TOKEN_NAME_KEY].replace(" ", "+"), "+", "@"));
        secret.prepend(QStringLiteral(TOKEN_TOKEN_KEY) + "=" + _tokenHash[TOKEN_TOKEN_KEY]);
        secret.prepend(QStringLiteral(TOKEN_TOKEN_SEC_KEY) + "=" + _tokenHash[TOKEN_TOKEN_SEC_KEY]);
        secret.prepend(QStringLiteral(TOKEN_CONSUMER_KEY) + "=" + _tokenHash[TOKEN_CONSUMER_KEY]);
        secret.prepend(QStringLiteral(TOKEN_CONSUMER_SEC_KEY) + "=" + _tokenHash[TOKEN_CONSUMER_SEC_KEY]);

        if (_tokenHash.contains(QStringLiteral(TOKEN_UPDATED_KEY))) {
            secret.prepend(QStringLiteral(TOKEN_UPDATED_KEY) + "=" + _tokenHash[TOKEN_UPDATED_KEY]);
        }
        if (_tokenHash.contains(QStringLiteral(TOKEN_CREATED_KEY))) {
            secret.prepend(QStringLiteral(TOKEN_CREATED_KEY) + "=" + _tokenHash[TOKEN_CREATED_KEY]);
        }

        secret.sort();

        return secret.join("&");
    }

    /**
     * \fn QString Token::consumerKey()
     *
     * Retruns a consumer key for this token, or empty string if consumer key is not set.
     */
    QString Token::consumerKey() const
    {
        return _tokenHash.value(TOKEN_CONSUMER_KEY, "");
    }

    /**
     * \fn bool Token::isValid()
     *
     * Check that the token is valid.
     **/
    bool Token::isValid() const
    {
        return (_tokenHash.contains(TOKEN_NAME_KEY) &&
                _tokenHash.contains(TOKEN_TOKEN_KEY) &&
                _tokenHash.contains(TOKEN_TOKEN_SEC_KEY) &&
                _tokenHash.contains(TOKEN_CONSUMER_KEY) &&
                _tokenHash.contains(TOKEN_CONSUMER_SEC_KEY));
    }

    /**
     * \fn QDateTime Token::created()
     *
     * Retruns a QDateTime representing the time the token was created on
     * the server, or empty if unknown.
     */
    QDateTime Token::created() const
    {
        if (_tokenHash.contains(QStringLiteral(TOKEN_CREATED_KEY))) {
            return QDateTime::fromString(_tokenHash[TOKEN_CREATED_KEY],
                                         Qt::ISODate);
        }
        return QDateTime();
    }

    /**
     * \fn QDateTime Token::updated()
     *
     * Returns a QDateTime representing the time the token was last updated on
     * the server, or empty if unknown.
     */
    QDateTime Token::updated() const
    {
        if (_tokenHash.contains(QStringLiteral(TOKEN_UPDATED_KEY))) {
            return QDateTime::fromString(_tokenHash[TOKEN_UPDATED_KEY],
                                         Qt::ISODate);
        }
        return QDateTime();
    }

    /**
     * \fn QDateTime Token::getServerTimestamp()
     *
     * Returns a QDateTime representing the current time known on the
     * Ubuntu One authentication server, or current local time on errors,
     * or when a QCoreApplication instance is not running.
     */
    QDateTime Token::getServerTimestamp() const
    {
        // The DateTime object to return, defaulting to current time
        QDateTime _dt = QDateTime::currentDateTimeUtc();

        // The timestamp of the client to send to the server.
        QDateTime _clientDate(_dt);
        auto _testDate = qgetenv("U1_TEST_TIMESTAMP");
        if (_testDate != "") {
            _clientDate = QDateTime::fromString(_testDate, Qt::RFC2822Date);
        }

        auto _app = QCoreApplication::instance();
        if (_app != nullptr) {
            QSharedPointer<QNetworkAccessManager> _nam(new QNetworkAccessManager());
            QUrl _url{SSOService::getAuthBaseUrl()}; 
            QSharedPointer<QNetworkRequest> _req(new QNetworkRequest(_url));
            _req->setRawHeader("Cache-Control", "no-cache");
            _req->setRawHeader("Date",
                               _clientDate.toString(Qt::RFC2822Date).toUtf8().data());

            qDebug() << "Getting timestamp from server:" << _url.toString();

            auto _reply = _nam->head(*_req);
            QEventLoop _loop;
            QObject::connect(_reply, &QNetworkReply::finished,
                             [&_loop, &_reply, &_dt]() {
                                 auto _date = _reply->rawHeader("Date");
                                 _dt = QDateTime::fromString(_date,
                                                             Qt::RFC2822Date);
                                 qDebug() << "Got server timestamp:"
                                          << _dt.toTime_t();
                                 _loop.quit();
                             });
            typedef void(QNetworkReply::*QNetworkReplyErrorSignal)(QNetworkReply::NetworkError);
            QObject::connect(_reply, static_cast<QNetworkReplyErrorSignal>(&QNetworkReply::error),
                             [&_loop, &_reply](QNetworkReply::NetworkError) {
                                 qCritical() << "Error fetching server timestamp:"
                                             << _reply->readAll();
                                 _loop.quit();
                             });

            _loop.exec();
        } else {
            qWarning() << "No main loop, defaulting to local timestamp.";
        }
        return _dt;
    }

    /**
     * \fn QUrl Token::addOAuthTimestamp(const QString url)
     *
     * Returns a QUrl with the oauth_timestamp value added to the query string
     */
    QUrl Token::addOAuthTimestamp(const QString url) const
    {
        // A copy of the URL in case we need to fix the timestamp
        QUrl newurl{url};

        // Get and use the server timestamp if necessary
        QDateTime _now = QDateTime::currentDateTimeUtc();
        // Static variables for caching the time to check, and skew
        static uint _ts_check = 0;
        static int _ts_skew = 0;

        QDateTime timestamp;
        if (_ts_check <= _now.toTime_t()) {
            timestamp = getServerTimestamp();
            _ts_skew = timestamp.toTime_t() - _now.toTime_t();
        } else {
            // QDateTime doesn't override + operator, create from time_t
            timestamp = QDateTime::fromTime_t(_now.toTime_t() + _ts_skew);
        }
        _ts_check = _now.toTime_t() + TIMESTAMP_CHECK_INTERVAL;
        char buf[11];
        snprintf(buf, 11, "%010u", timestamp.toTime_t());
        newurl.setQuery(newurl.query(QUrl::FullyEncoded) +
                        "&oauth_timestamp=" + buf);

        return newurl;
    }

    /**
     * \fn QString Token::signUrl(const QString url, const QString method,
     *                            bool asQuery = false)
     *
     * Sign a URL with the token.
     */
    QString Token::signUrl(const QString url, const QString method, bool asQuery) const
    {
        static const QString hdrPrefix("OAuth ");
        QString result;
        int argc = 0;
        char **argv = NULL;
        char *req_hdr = NULL;

        if (url.isEmpty()) {
            qWarning() << "Unable to sign empty URL.";
            return result;
        }

        QUrl copy(addOAuthTimestamp(url));
        argc = oauth_split_url_parameters(copy.toString().toUtf8().data(),
                                          &argv);
        // Fixup the URL as liboauth is escaping '+' to ' ' in it, incorrectly.
        for (int a = 0; argv[0][a] != 0; a++)
            argv[0][a] = argv[0][a] == ' ' ? '+' : argv[0][a];

        oauth_sign_array2_process(&argc, &argv, NULL,
                                  OA_HMAC, method.toUtf8().data(),
                                  _tokenHash[TOKEN_CONSUMER_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_CONSUMER_SEC_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_TOKEN_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_TOKEN_SEC_KEY].toUtf8().data());
        if (asQuery) {
            result = oauth_serialize_url_parameters(argc, argv);
        } else {
            req_hdr = oauth_serialize_url_sep(argc, 1, argv, (char *)", ", 6);
            result = hdrPrefix + QString(req_hdr);
        }
        oauth_free_array(&argc, &argv);
        free(req_hdr);

        return result;
    }

    /**
     * \fn Token Token::fromQuery(const QString query)
     *
     * Turn a query string for a token, into a Token object.
     **/
    Token *Token::fromQuery(const QString query)
    {
        Token *token = new Token();

        QStringList params = query.split("&");
        for (int i = 0; i < params.size(); ++i) {
            QStringList pair = params.at(i).split("=");
            if (pair.at(0) == TOKEN_NAME_KEY) {
                // TODO: Need to figure out how to actually use the
                // QUrl::fromPercentEncoding at this point in the code.
                QString value = pair.at(1);
                token->_tokenHash[pair.at(0)] = QString(value.replace("+", " ").replace("%40", "@"));
            } else if (pair.at(0) == TOKEN_UPDATED_KEY || pair.at(0) == TOKEN_CREATED_KEY) {
                token->_tokenHash[pair.at(0)] = dateStringToISO(pair.at(1));
            } else {
                token->_tokenHash[pair.at(0)] = pair.at(1);
            }
        }

        return token;
    }

    /**
     * \fn QString Token::buildTokenName()
     *
     * Compute the token name for the token, from the hostname.
     **/
    QString Token::buildTokenName()
    {
        QString computer_name = QHostInfo::localHostName();
        computer_name.replace(TOKEN_SEP, TOKEN_SEP_REPLACEMENT);

        return QStringLiteral(TOKEN_ID) + QStringLiteral(TOKEN_SEP) + computer_name;
    }

    /**
     * \fn QString Token::dateStringToISO(const QString date)
     *
     * Convert the date string from SSO server to ISO format, if needed.
     **/
    QString Token::dateStringToISO(const QString date)
    {
        // Force the date strings to be ISO formattted, not pythonish.
        // We can force GMT here, as we use it on the server.
        return QString(date).replace("+", "T").replace("%3A", ":").replace(QRegExp("\\.[0-9]+"), "Z");
    }

} // namespace UbuntuOne
