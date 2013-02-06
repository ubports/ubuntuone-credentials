#ifndef DBUS_HELPER_H
#define DBUS_HELPER_H

#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QHash>
#include <QObject>

typedef QHash<QString, QString> DBusStringHash;
typedef QHash<QString, QDBusObjectPath> DBusObjectPathHash;

namespace dbus
{

class DBusHelper : public QObject
{
    Q_OBJECT
public:
    static int DBUS_STRING_MAP_ID;
    static int DBUS_OBJECTPATH_MAP_ID;

    explicit DBusHelper(QObject *parent = 0);

    static class _init
    {
        public:
            _init()
            {
                // diff actions to init
                qRegisterMetaType<DBusStringHash>("DBusStringHash");
                qDBusRegisterMetaType<DBusStringHash>();
                DBUS_STRING_MAP_ID = QMetaType::type("DBusStringHash");

                qRegisterMetaType<DBusObjectPathHash>("DBusObjectPathHash");
                qDBusRegisterMetaType<DBusObjectPathHash>();
                DBUS_OBJECTPATH_MAP_ID = QMetaType::type("DBusObjectPathHash");
            }
    } _initializer;

    static QVariant getVariant(DBusStringHash hash);

};

} // dbus

Q_DECLARE_METATYPE(DBusStringHash)
Q_DECLARE_METATYPE(DBusObjectPathHash)

#endif // DBUS_HELPER_H
