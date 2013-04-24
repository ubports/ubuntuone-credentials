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

#ifndef INTERFACE_FACTORY_H
#define INTERFACE_FACTORY_H

#include "collection.h"
#include "item.h"
#include "service.h"

class InterfaceFactory
{

public:
    InterfaceFactory();
    ~InterfaceFactory();

    template <class T>
    T* create(const QString &service, const QString &path,
        const QDBusConnection &connection, QObject *parent = 0)
    {
        return new T(service, path, connection, parent);
    }

};

#endif // INTERFACE_FACTORY_H
