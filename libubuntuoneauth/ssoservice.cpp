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
#include <sys/utsname.h>

#include <QDebug>
#include <QtGlobal>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "logging.h"
#include "ssoservice.h"
#include "requests.h"
#include "errormessages.h"

namespace UbuntuOne {

    static Token _pendingPing;

    SSOService::SSOService(QObject *parent) :
        QObject(parent)
    {
        // Set up logging
        AuthLogger::setupLogging();
#if ENABLE_DEBUG
        AuthLogger::setLogLevel(QtDebugMsg);
#else
        if (qgetenv("U1_DEBUG") != "")
            AuthLogger::setLogLevel(QtDebugMsg);
#endif

        // create the keyring that will be used to store and retrieve the different tokens
        _keyring = new Keyring(this);
        _nam = new QNetworkAccessManager(this);

        connect(_keyring, SIGNAL(tokenFound(const Token&)),
                this, SLOT(credentialsAcquired(const Token&)));
        connect(_keyring, SIGNAL(tokenStored()),
                this, SLOT(credentialsSet()));
        connect(_keyring, SIGNAL(tokenDeleted()),
                this, SLOT(credentialsCleared()));

        connect(&(_provider),
                SIGNAL(OAuthTokenGranted(const OAuthTokenResponse&)),
                this, SLOT(tokenReceived(const OAuthTokenResponse&)));
        connect(&(_provider),
                SIGNAL(AccountGranted(const AccountResponse&)),
                this, SLOT(accountRegistered(const AccountResponse&)));
        connect(&(_provider),
                SIGNAL(ErrorOccurred(const ErrorResponse&)),
                this, SLOT(errorOcurred(const ErrorResponse&)));
    }

    void SSOService::getCredentials()
    {
        _keyring->findToken();
    }

    void SSOService::credentialsAcquired(const Token& token)
    {
        emit this->credentialsFound(token);
    }

    void SSOService::registerUser(QString email, QString password,
                                  QString display_name)
    {
        AccountRequest request(email, password, display_name, NULL, NULL);

        _tempPassword = password;
        _provider.CreateAccount(request);
    }

    void SSOService::accountRegistered(const AccountResponse& account)
    {
        login(account.email(), _tempPassword);
        _tempEmail = account.email();
        _tempPassword = "";
    }

    void SSOService::login(QString email, QString password)
    {
        OAuthTokenRequest request(email, password, Token::buildTokenName(), NULL);
        _tempEmail = email;

        _provider.GetOAuthToken(request);
    }

    static QString _getPlatformDataParams()
    {
        struct utsname _platData;
        uname(&_platData);

        QUrlQuery *params = new QUrlQuery();
        params->addQueryItem("platform", _platData.sysname);
        params->addQueryItem("platform_version", _platData.release);
        params->addQueryItem("platform_arch", _platData.machine);
        params->addQueryItem("client_version", PROJECT_VERSION);

        QString result = params->toString();
        qDebug() << "Ping parameters:" << result;
        return result;
    }

    QString SSOService::getAuthBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_AUTH_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://login.ubuntu.com/");
        return baseUrl;
    }

    QString SSOService::getUOneBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_UONE_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://one.ubuntu.com/");
        return baseUrl;
    }

    void SSOService::tokenReceived(const OAuthTokenResponse& token)
    {
        Token realToken = Token(token.token_key(), token.token_secret(),
                                token.consumer_key(), token.consumer_secret());
        QString urlToSign = getUOneBaseUrl() + "oauth/sso-finished-so-get-tokens/" + _tempEmail + "?" + _getPlatformDataParams();
        QString authHeader = realToken.signUrl(urlToSign,
                                               QStringLiteral("GET"));
        QNetworkRequest *_request = new QNetworkRequest();

        QObject::connect(_nam, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(accountPinged(QNetworkReply*)));

        _request->setRawHeader(QStringLiteral("Authorization").toUtf8(),
                              authHeader.toUtf8());
        _request->setUrl(urlToSign);
        _nam->get(*_request);

        _tempEmail = "";
        _pendingPing = realToken;
    }

    void SSOService::accountPinged(QNetworkReply* reply)
    {
        QVariant statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int httpStatus = statusAttr.toInt();

        if (reply->error() != QNetworkReply::NoError) {
            QVariant phraseAttr = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);
            QString reason = phraseAttr.toString();

            qCritical() << "Ping to Ubuntu One failed:" << httpStatus << reason;
            ErrorResponse error(httpStatus, reason, "", "");
            emit requestFailed(error);
        } else
            _keyring->storeToken(_pendingPing);

        reply->deleteLater();
        _pendingPing = Token();
    }

    void SSOService::invalidateCredentials()
    {
        _keyring->deleteToken();
    }

    void SSOService::errorOcurred(const ErrorResponse& error)
    {
        _tempPassword = "";
        emit requestFailed(error);
    }

}
