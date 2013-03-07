/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Manuel de la Pena <manuel.delapena@canonical.com>
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

#ifndef KEYRING_H
#define KEYRING_H

#include <QObject>
#include <QString>
#include "secret.h"

namespace keyring
{

class KeyringPrivate;
class Keyring : public QObject
{

    Q_OBJECT
    Q_DECLARE_PRIVATE(Keyring)
public:
    Keyring(QDBusConnection connection, QObject* parent=0, QString windowId = "0");
    ~Keyring();

    void openSession();
    void getCredentials(QString id);
    void setCredentials(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret);
    void deleteCredentials(QString id);

Q_SIGNALS:
    void userCancelation(QString id);
    void sessionOpened();
    void sessionOpenError();

    // signals used to let the user know that we did retrieve the data
    void credentialsFound(QString id, QString token, QString tokenSecret, QString consumer, QString consumerSecret, bool found);
    void credentialsError(QString id);
    void credentialsSet(QString id, bool stored);
    void credentialsDeleted(QString id, bool deleted);

private:
    Q_PRIVATE_SLOT(d_func(), void onSessionOpened(QDBusPendingCallWatcher*))
    Q_PRIVATE_SLOT(d_func(), void onSearchItemsForGet(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onSearchItemsForDelete(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onPromptCompletedForGet(bool, const QDBusVariant&, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onPromptCompletedForDelete(bool, const QDBusVariant&, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onCollectionUnlocked(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onReadAliasDone(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onItemDeleted(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onGetSecret(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onCreateItem(QDBusPendingCallWatcher*, QObject*))
    Q_PRIVATE_SLOT(d_func(), void onUnlock(QDBusPendingCallWatcher* , QObject*))



protected:
    // internal vars
    QScopedPointer<KeyringPrivate> d_ptr;
};

} // keyring

#endif // KEYRING_H
