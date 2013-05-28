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

#ifndef _IDENTITYPROVIDER_H_
#define _IDENTITYPROVIDER_H_

#include "network.h"
#include "requests.h"
#include "responses.h"

#include <QObject>

namespace UbuntuOne {

class IdentityProvider : public QObject
{
    Q_OBJECT
public:
    explicit IdentityProvider(QObject *parent = 0);

    void GetOAuthToken(const OAuthTokenRequest& token);
    void GetPasswordToken(const PasswordTokenRequest& token);
    void CreateAccount(const AccountRequest& account);

signals:
    void OAuthTokenGranted(const OAuthTokenResponse& token);
    void PasswordTokenGranted(const PasswordTokenResponse& token);
    void AccountGranted(const AccountResponse& account);
    void ErrorOccurred(const ErrorResponse& error);

public slots:
    void OnOAuthTokenGranted(const OAuthTokenResponse& token);
    void OnPasswordTokenGranted(const PasswordTokenResponse& token);
    void OnAccountGranted(const AccountResponse& account);
    void OnErrorOccurred(const ErrorResponse& error);

private slots:

private:
    Network* network;
};

} /* end SSO namespace */

#endif // _IDENTITYPROVIDER_H_
