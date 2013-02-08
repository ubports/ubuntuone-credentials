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
