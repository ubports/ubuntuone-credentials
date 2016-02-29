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

#include <Accounts/Account>
#include <Accounts/Service>
#include <SignOn/AuthSession>
#include <SignOn/Identity>
#include <SignOn/IdentityInfo>

#include <QDebug>

#include "accountmanager.h"
#include "authenticator.h"
#include "../signon-plugin/ubuntuonedata.h"

using namespace UbuntuOne;

Authenticator::Authenticator(QObject *parent):
    QObject(parent),
    m_manager(AccountManager::instance()),
    m_invalidate(false),
    m_uiAllowed(true)
{
}

void Authenticator::handleError(const SignOn::Error &e)
{
    qCritical() << "Authentication error:" << e.message();
    Q_EMIT error(AuthenticationError);
}

void Authenticator::handleSessionData(const SignOn::SessionData &data)
{
    PluginData reply = data.data<PluginData>();

    Token token(reply.TokenKey(), reply.TokenSecret(),
                reply.ConsumerKey(), reply.ConsumerSecret());
    if (token.isValid()) {
        Q_EMIT authenticated(token);
    } else {
        QString message("Failed to convert result to Token object.");
        qCritical() << message;
        Q_EMIT error(AuthenticationError);
    }
}

quint32 Authenticator::credentialsId()
{
    QString providerId("ubuntuone");
    Accounts::AccountIdList accountIds = m_manager->accountList(providerId);

    if (accountIds.isEmpty()) {
        qDebug() << "authenticate(): No UbuntuOne accounts found";
        Q_EMIT error(AccountNotFound);
        return 0;
    }

    if (accountIds.count() > 1) {
        qWarning() << "authenticate(): Found '" << accountIds.count() <<
            "' accounts. Using first.";
    }

    qDebug() << "authenticate(): Using account '" << accountIds[0] << "'.";

    auto account = m_manager->account(accountIds[0]);
    if (Q_UNLIKELY(!account)) {
        qDebug() << "Couldn't load account";
        /* This could either happen because the account was deleted right while
         * we were loading it, or because the accounts DB was locked by another
         * app. Let's just return an authentication error here, so the client
         * can retry.
         */
        Q_EMIT error(AuthenticationError);
        return 0;
    }

    /* Here we should check that the account service is enabled; but since the
     * old code was not doing this check, and that from the API there is no way
     * of knowing which service we are interested in, let's leave it as a TODO.
     */

    return account->credentialsId();
}

void Authenticator::authenticate(const QString &tokenName,
                                 const QString &userName,
                                 const QString &password,
                                 const QString &otp)
{
    SignOn::Identity *identity;
    if (userName.isEmpty()) {
        // Use existing account
        quint32 id = credentialsId();
        if (Q_UNLIKELY(!id)) return;

        identity = SignOn::Identity::existingIdentity(id, this);
        if (Q_UNLIKELY(!identity)) {
            qCritical() << "authenticate(): unable to load credentials" << id;
            Q_EMIT error(AccountNotFound);
            return;
        }
    } else {
        identity = SignOn::Identity::newIdentity(SignOn::IdentityInfo(), this);
    }

    auto session = identity->createSession(QStringLiteral("ubuntuone"));
    if (Q_UNLIKELY(!session)) {
        qCritical() << "Unable to create AuthSession.";
        Q_EMIT error(AuthenticationError);
        return;
    }

    connect(session, SIGNAL(response(const SignOn::SessionData&)),
            this, SLOT(handleSessionData(const SignOn::SessionData&)));
    connect(session, SIGNAL(error(const SignOn::Error&)),
            this, SLOT(handleError(const SignOn::Error&)));

    PluginData data;
    data.setTokenName(tokenName);
    data.setUserName(userName);
    data.setSecret(password);
    data.setOneTimePassword(otp);
    int uiPolicy = m_uiAllowed ?
        SignOn::DefaultPolicy : SignOn::NoUserInteractionPolicy;
    if (m_invalidate) {
        uiPolicy |= SignOn::RequestPasswordPolicy;
        m_invalidate = false;
    }
    data.setUiPolicy(uiPolicy);

    session->process(data, QStringLiteral("ubuntuone"));
}

void Authenticator::invalidateCredentials()
{
    m_invalidate = true;
}

void Authenticator::setUiAllowed(bool allowed)
{
    m_uiAllowed = allowed;
}
