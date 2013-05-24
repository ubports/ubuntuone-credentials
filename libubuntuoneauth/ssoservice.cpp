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

#include <QDebug>
#include <QtGlobal>

#include "logging.h"
#include "ssoservice.h"
#include "sso_api/requests.h"
#include "sso_api/errormessages.h"

namespace UbuntuOne {

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
        _keyring = new Keyring();
        connect(_keyring, SIGNAL(tokenFound(const Token&)),
                this, SLOT(credentialsAcquired(const Token&)));

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
        qDebug() << "Found a token.";
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
        _tempPassword = "";
    }

    void SSOService::login(QString email, QString password)
    {
        OAuthTokenRequest request(email, password, Token::buildTokenName(), NULL);

        _provider.GetOAuthToken(request);
    }

    void SSOService::tokenReceived(const OAuthTokenResponse& token)
    {
        Token realToken = Token(token.token_key(), token.token_secret(),
                                token.consumer_key(), token.consumer_secret());
        _keyring->storeToken(realToken);
        emit credentialsFound(realToken);
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
