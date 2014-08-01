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

#include "keyring.h"

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
        QString secret = data.Secret();

        if (secret.length() == 0) {
            QString msg("Could not read credentials secret value.");
            qCritical() << msg;
            emit keyringError(msg);
            return;
        }

        Token *token = Token::fromQuery(secret);
        if (token->isValid()) {
            emit tokenFound(*token);
        } else {
            QString message("Failed to convert result to Token object.");
            qCritical() << message;
            emit keyringError(message);
        }
        delete token;
    }

    void Keyring::findToken()
    {
        QString _acctName("ubuntuone");
        AccountIdList _ids = _manager.accountList(_acctName);
        Identity *identity;
        Account *account;

        if (_ids.length() > 0) {
            if (_ids.length() > 1) {
                qDebug() << "findToken(): Found '" << _ids.length() << "' accounts. Using first.";
            }
            account = _manager.account(_ids[0]);
            qDebug() << "findToken(): Using Ubuntu One account '" << _ids[0] << "'.";
            identity = Identity::existingIdentity(account->credentialsId());
            if (identity == NULL) {
                qCritical() << "findToken(): disabled account " << _acctName << _ids[0];
                emit tokenNotFound();
                return;
            }
            AuthSession *session = identity->createSession(QStringLiteral("password"));
            if (session != NULL) {
                connect(session, SIGNAL(response(const SignOn::SessionData&)),
                        this, SLOT(handleSessionData(const SignOn::SessionData&)));
                connect(session, SIGNAL(error(const SignOn::Error&)),
                        this, SLOT(handleError(const SignOn::Error&)));
                session->process(SessionData(), QStringLiteral("password"));
                return;
            }
            qCritical() << "Unable to create AuthSession.";
        }
        qDebug() << "findToken(): No accounts found matching " << _acctName;
        emit tokenNotFound();
    }

    void Keyring::handleCredentialsStored(const quint32 id)
    {
        QString _acctName("ubuntuone");
        AccountIdList _ids = _manager.accountList(_acctName);

        _account->selectService();
        _account->setCredentialsId(id);

        ServiceList services = _account->services(_acctName);
        if (services.length() > 0) {
            _account->selectService(services[0]);
        } else {
            QString errMsg("Unable to enable 'ubuntuone' service.");
            emit keyringError(errMsg);
        }
        _account->setEnabled(true);
        emit tokenStored();
    }

    void Keyring::storeToken(Token token)
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

            ServiceList services = _account->services(_acctName);
            if (services.length() > 0) {
                _account->selectService(services[0]);
            } else {
                QString errMsg("Unable to enable 'ubuntuone' service.");
                emit keyringError(errMsg);
            }

            _account->setEnabled(true);
        }

        if(_account->credentialsId() == 0){
            qDebug() << "storeToken() : creating new Identity for account " << _account->id() ;
            identity = Identity::newIdentity();
        }else{
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
        identity->storeCredentials(info);
    }

    void Keyring::handleAccountRemoved()
    {
        emit tokenDeleted();
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
                    this, SLOT(handleError(const SignOn::Error&)));

            identity->remove();
            account->remove();
            account->sync();
            return;
        }
        emit tokenNotFound();
    }

} // namespace UbuntuOne
