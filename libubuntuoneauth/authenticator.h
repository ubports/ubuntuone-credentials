/*
 * Copyright 2016 Canonical Ltd.
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
#ifndef _U1_AUTHENTICATOR_H_
#define _U1_AUTHENTICATOR_H_

#include <Accounts/Manager>
#include <SignOn/Identity>

#include <QObject>

#include "token.h"


namespace UbuntuOne {

class Authenticator : public QObject
{
    Q_OBJECT

public:
    enum ErrorCode {
        NoError = 0,
        AccountNotFound,
        OneTimePasswordRequired,
        InvalidPassword,
        AuthenticationError, // will create more specific codes if needed
    };

    explicit Authenticator(QObject *parent = 0);

    void authenticate(const QString &tokenName,
                      const QString &userName = QString(),
                      const QString &password = QString(),
                      const QString &otp = QString());
    void invalidateCredentials();
    void setUiAllowed(bool allowed);

Q_SIGNALS:
    void authenticated(const Token& token);
    void error(UbuntuOne::Authenticator::ErrorCode code);

private:
    quint32 credentialsId();

private Q_SLOTS:
    void handleError(const SignOn::Error &error);
    void handleSessionData(const SignOn::SessionData &data);

private:
    Accounts::Manager *m_manager;
    bool m_invalidate;
    bool m_uiAllowed;
};

} /* namespace UbuntuOne */

#endif /* _U1_AUTHENTICATOR_H_ */
