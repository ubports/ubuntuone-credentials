/*
 * Copyright 2013-2014 Canonical Ltd.
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
#ifndef _U1_KEYRING_H_
#define _U1_KEYRING_H_

#include <Accounts/Account>
#include <Accounts/Manager>
#include <SignOn/Identity>

#include <QObject>

#include "common.h"
#include "token.h"


namespace UbuntuOne {

    class U1_EXPORT Keyring : public QObject
    {
        Q_OBJECT
    public:
        explicit Keyring(QObject *parent=NULL);

        void findToken();
        void storeToken(Token token);
        void storeToken(Token token, const QString& displayName);
        void deleteToken();

    Q_SIGNALS:
        void tokenFound(const Token& token);
        void tokenNotFound();
        void tokenStored();
        void tokenDeleted();

        void keyringError(QString message);

    private Q_SLOTS:
        void handleError(const SignOn::Error &error);
        void handleSessionData(const SignOn::SessionData &data);
        void handleCredentialsStored(const quint32 id);
        void handleAccountRemoved();
        void handleDeleteError(const SignOn::Error &error);

    private:
        Accounts::Manager _manager;
        Accounts::Account *_account;
    };

} /* namespace UbuntuOne */

#endif /* _U1_KEYRING_H_ */
