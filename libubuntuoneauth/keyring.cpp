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
#include <Accounts/Account>
#include <Accounts/AccountService>
#include <Accounts/Service>
#include <Accounts/ServiceType>
#include <SignOn/Identity>

#include <QDebug>

#include "authenticator.h"
#include "keyring.h"
#include "../signon-plugin/ubuntuonedata.h"

using namespace Accounts;
using namespace SignOn;


namespace UbuntuOne {

    Keyring::Keyring(QObject *parent)
        : QObject(parent),
          _manager()
    {
        _account = nullptr;
    }

    void Keyring::handleError(const SignOn::Error &error)
    {
        qCritical() << "Error:" << error.message();
        emit keyringError(error.message());
    }

    void Keyring::handleSessionData(const SignOn::SessionData &data)
    {
        PluginData reply = data.data<PluginData>();

        Token token(reply.TokenKey(), reply.TokenSecret(),
                    reply.ConsumerKey(), reply.ConsumerSecret());
        if (token.isValid()) {
            emit tokenFound(token);
        } else {
            QString message("Failed to convert result to Token object.");
            qCritical() << message;
            emit keyringError(message);
        }
    }

    void Keyring::findToken()
    {
        auto authenticator = new Authenticator(this);
        authenticator->setUiAllowed(false);

        connect(authenticator, &Authenticator::authenticated,
                [=](const Token &token) {
            Q_EMIT tokenFound(token);
            authenticator->deleteLater();
        });
        connect(authenticator, &Authenticator::error,
                [=](Authenticator::ErrorCode code) {
            if (code == Authenticator::AccountNotFound) {
                Q_EMIT tokenNotFound();
            } else {
                Q_EMIT keyringError("Authentication failed");
            }
            authenticator->deleteLater();
        });
        authenticator->authenticate(Token::buildTokenName());
    }

    void Keyring::handleCredentialsStored(const quint32 id)
    {
        QString _acctName("ubuntuone");
        AccountIdList _ids = _manager.accountList(_acctName);

        _account->selectService();
        _account->setCredentialsId(id);

        for (auto service: _account->services()) {
            _account->selectService(service);
            _account->setEnabled(true);
        }
        _account->sync();
        emit tokenStored();
    }

    void Keyring::storeToken(Token token)
    {
        storeToken(token, "");
    }

    void Keyring::storeToken(Token token, const QString& displayName)
    {
        QString _acctName("ubuntuone");
        AccountIdList _ids = _manager.accountList(_acctName);
        Identity *identity = NULL;

        if (_ids.length() > 0) {
            if (_ids.length() > 1) {
                qDebug() << "storeToken(): Found '" << _ids.length() << "' accounts. Using first.";
            }
            _account = _manager.account(_ids[0]);
            qDebug() << "storeToken(): Using Ubuntu One account '" << _ids[0] << "'.";
        } else {
            qDebug() << "in storeToken(): no accounts found in accountList, creating new";
            _account = _manager.createAccount(_acctName);
        }
        _account->setEnabled(true);

        if (!displayName.isEmpty()) {
            _account->setDisplayName(displayName);
        }

        if(_account->credentialsId() == 0) {
            qDebug() << "storeToken() : creating new Identity for account " << _account->id() ;
            identity = Identity::newIdentity();
        } else {
            qDebug() << "storeToken(): identity found.";
            identity = Identity::existingIdentity(_account->credentialsId());
        }

        Q_ASSERT(identity != NULL);

        connect(identity, SIGNAL(error(const SignOn::Error&)),
                this, SLOT(handleError(const SignOn::Error&)));
        connect(identity, SIGNAL(credentialsStored(const quint32)),
                this, SLOT(handleCredentialsStored(const quint32)));

        IdentityInfo info = IdentityInfo();

        info.setSecret(token.toQuery(), true);
        info.setAccessControlList(QStringList() << "unconfined");
        identity->storeCredentials(info);
    }

    void Keyring::handleAccountRemoved()
    {
        emit tokenDeleted();
    }

    void Keyring::handleDeleteError(const SignOn::Error &error)
    {
        // Just log the error here, as we don't want to infinite loop.
        qWarning() << "Error deleting token:" << error.message();
    }

    void Keyring::deleteToken()
    {
        QString _acctName("ubuntuone");
        AccountIdList _ids = _manager.accountList(_acctName);
        if (_ids.length() > 0) {
            if (_ids.length() > 1) {
                qDebug() << "deleteToken(): Found '" << _ids.length() << "' accounts. Using first.";
            }
            Account *account = _manager.account(_ids[0]);
            qDebug() << "deleteToken(): Using Ubuntu One account '" << _ids[0] << "'.";
            Identity *identity = Identity::existingIdentity(account->credentialsId());
            connect(account, SIGNAL(removed()),
                    this, SLOT(handleAccountRemoved()));
            connect(identity, SIGNAL(error(const SignOn::Error&)),
                    this, SLOT(handleDeleteError(const SignOn::Error&)));

            identity->remove();
            account->remove();
            account->sync();
            return;
        }
        emit tokenNotFound();
    }

} // namespace UbuntuOne
