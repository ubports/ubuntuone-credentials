/*
 * Copyright 2013 Canonical Ltd.
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
#include <stdlib.h>
#include <oauth.h>

#include <QHash>
#include <QHostInfo>
#include <QStringList>
#include <QUrl>

#include "token.h"

#define TOKEN_NAME_KEY "name"
#define TOKEN_TOKEN_KEY "token"
#define TOKEN_TOKEN_SEC_KEY "token_secret"
#define TOKEN_CONSUMER_KEY "consumer_key"
#define TOKEN_CONSUMER_SEC_KEY "consumer_secret"

namespace UbuntuOne {

    Token::Token(QString token_key, QString token_secret,
                 QString consumer_key, QString consumer_secret)
    {
        _tokenHash[TOKEN_NAME_KEY] = buildTokenName();
        _tokenHash[TOKEN_TOKEN_KEY] = token_key;
        _tokenHash[TOKEN_TOKEN_SEC_KEY] = token_secret;
        _tokenHash[TOKEN_CONSUMER_KEY] = consumer_key;
        _tokenHash[TOKEN_CONSUMER_SEC_KEY] = consumer_secret;
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

        secret.sort();

        return secret.join("&");
    }

    /**
     * \fn bool Token::isValid()
     *
     * Check that the token is valid.
     **/
    bool Token::isValid()
    {
        return (_tokenHash.contains(TOKEN_NAME_KEY) &&
                _tokenHash.contains(TOKEN_TOKEN_KEY) &&
                _tokenHash.contains(TOKEN_TOKEN_SEC_KEY) &&
                _tokenHash.contains(TOKEN_CONSUMER_KEY) &&
                _tokenHash.contains(TOKEN_CONSUMER_SEC_KEY));
    }

    /**
     * \fn QString Token::signUrl(const QString url, const QString method,
     *                            bool asQuery = false)
     *
     * Sign a URL with the token.
     */
    QString Token::signUrl(const QString url, const QString method, bool asQuery)
    {
        static const QString hdrPrefix("OAuth ");
        QString result;
        int argc = 0;
        char **argv = NULL;

        argc = oauth_split_url_parameters(url.toUtf8().data(), &argv);
        oauth_sign_array2_process(&argc, &argv, NULL,
                                  OA_HMAC, method.toUtf8().data(),
                                  _tokenHash[TOKEN_CONSUMER_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_CONSUMER_SEC_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_TOKEN_KEY].toUtf8().data(),
                                  _tokenHash[TOKEN_TOKEN_SEC_KEY].toUtf8().data());
        if (asQuery)
            result = oauth_serialize_url_parameters(argc, argv);
        else
            result = hdrPrefix + oauth_serialize_url_sep(argc, 1, argv, (char *)", ", 6);
        oauth_free_array(&argc, &argv);

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
            } else
                token->_tokenHash[pair.at(0)] = pair.at(1);
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

} // namespace UbuntuOne
