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

#include "dbus_helper.h"

namespace dbus
{

// required for the init
int DBusHelper::DBUS_STRING_MAP_ID = 0;
int DBusHelper::DBUS_OBJECTPATH_MAP_ID = 1;

DBusHelper::_init DBusHelper::_initializer;

DBusHelper::DBusHelper(QObject *parent) :
    QObject(parent)
{
}


QVariant DBusHelper::getVariant(DBusStringHash hash)
{
    return QVariant(DBUS_STRING_MAP_ID, &hash);
}

} // dbus
