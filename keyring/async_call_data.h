#ifndef ASYNC_CALL_DATA_H
#define ASYNC_CALL_DATA_H

#include <functional>
#include <QDBusObjectPath>
#include <QObject>
#include <QString>

namespace keyring
{

// class used to pass extra information to the dbus signal mapper
class AsyncCallData : public QObject
{

    typedef std::function<void(AsyncCallData*)> async_callback_cb;
    typedef std::function<void(QString, AsyncCallData*)> locked_items_cb;

    Q_OBJECT
public:
    AsyncCallData(QObject* parent=0) : QObject(parent) {}

    QString accId;
    QString token;
    QString tokenSecret;
    QString interfacePath;
    QList<QDBusObjectPath> unlocked;
    QList<QDBusObjectPath> locked;
    async_callback_cb unlockedItemsCb;
    async_callback_cb unlockDbusError;
    locked_items_cb lockedItemsCb;
};

} // keyring

#endif // ASYNC_CALL_DATA_H
