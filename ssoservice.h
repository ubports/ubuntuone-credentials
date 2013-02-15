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
    void tokenReceived(const TokenResponse& token);
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
