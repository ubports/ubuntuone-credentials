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

#include "authenticator.h"
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
                this, SLOT(handleCredentialsFound(const Token&)));
        connect(_keyring, SIGNAL(tokenNotFound()),
                this, SLOT(handleCredentialsNotFound()));

        connect(_keyring, SIGNAL(tokenStored()),
                this, SLOT(handleTokenStored()));
        connect(_keyring, SIGNAL(tokenDeleted()),
                this, SLOT(handleTokenDeleted()));

        connect(_keyring, SIGNAL(keyringError(QString)),
                this, SLOT(handleKeyringError(QString)));

        connect(_nam, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(accountPinged(QNetworkReply*)));

        connect(&(_provider),
                SIGNAL(AccountGranted(const AccountResponse&)),
                this, SLOT(accountRegistered(const AccountResponse&)));
        connect(&(_provider),
                SIGNAL(TwoFactorAuthRequired()),
                this, SLOT(handleTwoFactorAuthRequired()));
        connect(&(_provider),
                SIGNAL(ErrorOccurred(const ErrorResponse&)),
                this, SLOT(errorOccurred(const ErrorResponse&)));
    }

    void SSOService::getCredentials()
    {
        _keyring->findToken();
    }

    void SSOService::handleCredentialsNotFound()
    {
        emit credentialsNotFound();
    }
    
    void SSOService::handleCredentialsFound(const Token& token)
    {
        emit credentialsFound(token);
    }

    void SSOService::handleTokenStored()
    {
        emit credentialsStored(); 
    }

    void SSOService::registerUser(QString email, QString password,
                                  QString display_name)
    {
        AccountRequest request(getAuthBaseUrl(),
                               email, password, display_name);

        _tempPassword = password;
        _tempEmail = email;
        _provider.CreateAccount(request);
    }

    void SSOService::accountRegistered(const AccountResponse& account)
    {
        login(_tempEmail, _tempPassword);
        _tempPassword = "";
    }

    void SSOService::login(QString email, QString password, QString twoFactorCode)
    {
        auto authenticator = new Authenticator(this);
        authenticator->setUiAllowed(false);

        connect(authenticator, &Authenticator::authenticated,
                [=](const Token &token) {
            _keyring->storeToken(token, email);
            authenticator->deleteLater();
        });
        connect(authenticator, &Authenticator::error,
                [=](Authenticator::ErrorCode code) {
            if (code == Authenticator::AccountNotFound) {
                Q_EMIT credentialsNotFound();
            } else {
                /* TODO: deliver a proper error response. */
                Q_EMIT requestFailed(ErrorResponse());
            }
            authenticator->deleteLater();
        });
        authenticator->authenticate(Token::buildTokenName(),
                                    email, password, twoFactorCode);
    }

    void SSOService::handleTwoFactorAuthRequired()
    {
        emit twoFactorAuthRequired();
    }

    QString SSOService::getAuthBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_AUTH_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://login.ubuntu.com");
        return baseUrl;
    }

    QString SSOService::getUOneBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_UONE_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://one.ubuntu.com");
        return baseUrl;
    }

    void SSOService::tokenReceived(const OAuthTokenResponse& token)
    {
        // Not used anymore
    }

    void SSOService::accountPinged(QNetworkReply*)
    {
        /* DEPRECATED */
    }

    void SSOService::invalidateCredentials()
    {
        _keyring->deleteToken();
    }

    void SSOService::errorOccurred(const ErrorResponse& error)
    {
        _tempPassword = "";
        emit requestFailed(error);
    }

    void SSOService::handleKeyringError(QString message)
    {
        _tempPassword = "";
        /* Treat keyring errors as invalid/expired token.
           Delete the token and act like it wasn't found. */
        invalidateCredentials();
        emit credentialsNotFound();
    }
}
