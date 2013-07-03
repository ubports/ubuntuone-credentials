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

#include "identityprovider.h"
#include "responses.h"


namespace UbuntuOne {

IdentityProvider::IdentityProvider(QObject *parent) :
    QObject(parent)
{
    /* TODO: either shared pointer or delete this */
    this->network = new Network(this);

    QObject::connect(this->network,
                     SIGNAL(OAuthTokenGranted(const OAuthTokenResponse&)),
                     this, SLOT(OnOAuthTokenGranted(const OAuthTokenResponse&)));
    QObject::connect(this->network,
                     SIGNAL(PasswordTokenGranted(const PasswordTokenResponse&)),
                     this, SLOT(OnPasswordTokenGranted(const PasswordTokenResponse&)));
    QObject::connect(this->network, SIGNAL(AccountGranted(const AccountResponse&)),
                     this, SLOT(OnAccountGranted(const AccountResponse&)));
    QObject::connect(this->network, SIGNAL(ErrorOccurred(const ErrorResponse&)),
                     this, SLOT(OnErrorOccurred(const ErrorResponse&)));
    }

void IdentityProvider::GetOAuthToken(const OAuthTokenRequest& token)
{
    this->network->Post(token);
}

void IdentityProvider::GetPasswordToken(const PasswordTokenRequest& token)
{
    this->network->Post(token);
}

void IdentityProvider::CreateAccount(const AccountRequest& account)
{
    this->network->Post(account);
}

/* Token API slots */

void IdentityProvider::OnOAuthTokenGranted(const OAuthTokenResponse& token)
{
    qWarning() << "OAuth token received for " << token.token_name();

    emit OAuthTokenGranted(token);
}

void IdentityProvider::OnPasswordTokenGranted(const PasswordTokenResponse& token)
{
    qWarning() << "Password token received for " << token.email();

    emit PasswordTokenGranted(token);
}

void IdentityProvider::OnAccountGranted(const AccountResponse& account)
{
    qWarning() << "Account created for " << account.email(); 

    emit AccountGranted(account);
}

void IdentityProvider::OnErrorOccurred(const ErrorResponse& error)
{
    qWarning("Error occurred creating account: %d (%s)",
             error.code(), error.message().toUtf8().data());

    emit ErrorOccurred(error);

}

} /* end UbuntuOne namespace */
