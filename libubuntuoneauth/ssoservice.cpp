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

#include "ssoservice.h"
#include <QDebug>
#include "sso_api/requests.h"

namespace SSO {

#define TOKEN_NAME "UbuntuSSO"

SSOService::SSOService(QObject *parent) :
    QObject(parent),
    _conn(QDBusConnection::sessionBus())
{
    // create the keyring that will be used to store and retrieve the different tokens
    _keyring = new keyring::Keyring(_conn);
    this->connect(_keyring, SIGNAL(sessionOpened()), this, SLOT(sessionDetected()));
    this->connect(_keyring, SIGNAL(credentialsFound(QString,QString,QString,QString,QString,bool)),
                  this, SLOT(credentialsAcquired(QString,QString,QString,QString,QString,bool)));
    this->connect(_keyring, SIGNAL(credentialsSet(QString, bool)), this, SLOT(credentialsSet(QString, bool)));

    this->connect(&(this->provider), SIGNAL(OAuthTokenGranted(const OAuthTokenResponse&)), this, SLOT(tokenReceived(const OAuthTokenResponse&)));
    this->connect(&(this->provider), SIGNAL(AccountGranted(const AccountResponse&)), this, SLOT(accountRegistered(const AccountResponse&)));
    this->connect(&(this->provider), SIGNAL(ErrorOccurred(const ErrorResponse&)), this, SLOT(errorOcurred(const ErrorResponse&)));
}

void SSOService::init_service()
{
    _keyring->openSession();
}

bool SSOService::sessionOpened()
{
    return this->_serviceEnabled;
}

void SSOService::sessionDetected()
{
    this->_serviceEnabled = true;
    emit this->sessionActivated();
}

void SSOService::getCredentials()
{
    this->_keyring->getCredentials(TOKEN_NAME);
}

void SSOService::credentialsAcquired(QString id, QString token, QString secret, QString consumer, QString consumerSecret, bool found)
{
    if(found)
    {
        emit this->credentialsFound(id, token, secret, consumer, consumerSecret);
    }
    else
    {
        emit this->credentialsNotFound(id);
    }

}

void SSOService::registerUser(QString email, QString password, QString display_name)
{
    SSO::AccountRequest request(email, password, display_name, NULL, NULL);

    this->_tempPassword = password;
    this->provider.CreateAccount(request);
}

void SSOService::accountRegistered(const AccountResponse& account)
{
    this->login(account.email(), this->_tempPassword);
    this->_tempPassword = "";
}

void SSOService::login(QString email, QString password)
{
    OAuthTokenRequest request(email, password, TOKEN_NAME, NULL);

    this->provider.GetOAuthToken(request);
}

void SSOService::tokenReceived(const OAuthTokenResponse& token)
{
    // Save the Tokens in the Keyring
    this->_keyring->setCredentials(TOKEN_NAME, token.token_key(), token.token_secret(),
                                   token.consumer_key(), token.consumer_secret());
}

void SSOService::credentialsSet(QString id, bool stored)
{
    if(stored)
    {
        this->_keyring->getCredentials(TOKEN_NAME);
    }
    else
    {
        emit this->loginFailed("Failed to set credentials");
    }
}

void SSOService::invalidateCredentials()
{
    this->_keyring->deleteCredentials(TOKEN_NAME);
    emit credentialsDeleted();
}

void SSOService::errorOcurred(const ErrorResponse& error)
{
    this->_tempPassword = "";
    emit this->requestFailed(error);
}

}