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
    void setCredentials(QString id, QString token, QString tokenSecret);
    void deleteCredentials(QString id);

Q_SIGNALS:
    void userCancelation(QString id);
    void sessionOpened();
    void sessionOpenError();

    // signals used to let the user know that we did retrieve the data
    void credentialsFound(QString id, QString token, QString tokenSecret, bool found);
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
