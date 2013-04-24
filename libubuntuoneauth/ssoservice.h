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

#ifndef SSOSERVICE_H
#define SSOSERVICE_H

#include <QObject>
#include <QString>
#include "keyring/keyring.h"
#include "sso_api/identityprovider.h"
#include "sso_api/responses.h"

namespace SSO {

class SSOService : public QObject
{
    Q_OBJECT
public:
    explicit SSOService(QObject *parent = 0);

    void init_service();
    bool sessionOpened();
    void invalidateCredentials();
    void getCredentials();
    void login(QString email, QString password);
    void registerUser(QString email, QString password, QString display_name);

signals:
    void sessionActivated();
    void credentialsDeleted();

    void credentialsFound(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret);
    void credentialsNotFound(QString id);
    void loginFailed(QString message);
    void requestFailed(const ErrorResponse& error);

private slots:
    void sessionDetected();
    void credentialsSet(QString id, bool stored);
    void credentialsAcquired(QString id, QString token, QString secret, QString consumer, QString consumerSecret, bool found);
    void tokenReceived(const OAuthTokenResponse& token);
    void accountRegistered(const AccountResponse& account);
    void errorOcurred(const ErrorResponse&);

private:
    bool _serviceEnabled = false;
    QString _tempPassword;
    QDBusConnection _conn;
    keyring::Keyring* _keyring;
    IdentityProvider provider;
};

}

#endif // SSOSERVICE_H
