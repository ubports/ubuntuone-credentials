/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or
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
#include <Accounts/Manager>

#include <QCoreApplication>
#include <QDebug>

#include <SignOn/Identity>
#include <SignOn/IdentityInfo>

#include "acl-updater.h"

AclUpdater::AclUpdater(quint32 credentialsId, QObject *parent):
    QObject(parent),
    identity(0)
{
    qDebug() << "Loading identity" << credentialsId;

    identity = SignOn::Identity::existingIdentity(credentialsId, this);
    QObject::connect(identity, SIGNAL(info(const SignOn::IdentityInfo&)),
                     this, SLOT(onInfoReady(const SignOn::IdentityInfo&)));
    QObject::connect(identity, SIGNAL(error(const SignOn::Error&)),
                     this, SLOT(onError(const SignOn::Error&)));
    identity->queryInfo();
}

void AclUpdater::onInfoReady(const SignOn::IdentityInfo &info)
{
    QStringList currentAcl = info.accessControlList();
    QString unconfined = QStringLiteral("unconfined");

    if (currentAcl.contains(unconfined)) {
        // nothing to do
        qDebug() << "ACL already contains 'unconfined'";
        Q_EMIT finished();
        return;
    }

    qDebug() << "Updating identity";
    SignOn::IdentityInfo updatedInfo(info);
    updatedInfo.setAccessControlList(currentAcl << unconfined);

    QObject::connect(identity, SIGNAL(credentialsStored(const quint32)),
                     this, SIGNAL(finished()));
    identity->storeCredentials(updatedInfo);
}

void AclUpdater::onError(const SignOn::Error &error)
{
    qWarning() << "Error updating identity" << identity->id() <<
        ":" << error.message();
    Q_EMIT finished();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    Accounts::Manager manager;
    AclUpdater *updater = 0;

    Q_FOREACH(Accounts::AccountId accountId, manager.accountList()) {
        Accounts::Account *account = manager.account(accountId);
        if (account->providerName() == QStringLiteral("ubuntuone")) {
            updater = new AclUpdater(account->credentialsId());
            break;
        }
    }

    if (updater) {
        QObject::connect(updater, SIGNAL(finished()),
                         &app, SLOT(quit()));
        return app.exec();
    } else {
        return EXIT_SUCCESS;
    }
}
