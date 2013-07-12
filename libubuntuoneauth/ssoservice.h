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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

#include "keyring.h"
#include "token.h"

#include "identityprovider.h"
#include "responses.h"

namespace UbuntuOne {

    class SSOService : public QObject
    {
        Q_OBJECT
    public:
        explicit SSOService(QObject *parent = 0);

        void invalidateCredentials();
        void getCredentials();
        void login(QString email, QString password);
        void registerUser(QString email, QString password,
                          QString display_name);

        static QString getAuthBaseUrl();
        static QString getUOneBaseUrl();

    signals:
        void credentialsDeleted();
        void credentialsStored();
        void credentialsFound(const Token& token);
        void credentialsNotFound();
        void requestFailed(const ErrorResponse& error);

        private slots:
            void accountPinged(QNetworkReply*);
            void handleTokenStored();
            void handleTokenDeleted() { emit credentialsDeleted(); };
            void handleCredentialsFound(const Token& token);
            void handleCredentialsNotFound();
            void tokenReceived(const OAuthTokenResponse& token);
            void accountRegistered(const AccountResponse& account);
            void errorOcurred(const ErrorResponse&);

    private:
            Keyring *_keyring;

            QString _tempPassword;
            QString _tempEmail;
            IdentityProvider _provider;
            QNetworkAccessManager *_nam;
    };

}

#endif // SSOSERVICE_H
