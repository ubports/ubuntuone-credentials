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
    QString consumer;
    QString consumerSecret;
    QString interfacePath;
    QList<QDBusObjectPath> unlocked;
    QList<QDBusObjectPath> locked;
    async_callback_cb unlockedItemsCb;
    async_callback_cb unlockDbusError;
    locked_items_cb lockedItemsCb;
};

} // keyring

#endif // ASYNC_CALL_DATA_H
