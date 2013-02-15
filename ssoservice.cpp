/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Diego Sarmentero <diego.sarmentero@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

    this->connect(&(this->provider), SIGNAL(TokenGranted(const TokenResponse&)), this, SLOT(tokenReceived(const TokenResponse&)));
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
    TokenRequest request(email, password, TOKEN_NAME, NULL);

    this->provider.GetToken(request);
}

void SSOService::tokenReceived(const TokenResponse& token)
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
