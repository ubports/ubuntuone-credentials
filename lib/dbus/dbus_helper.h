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

#endif // DBUS_HELPER_H
