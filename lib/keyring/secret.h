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

#ifndef SECRET_H
#define SECRET_H

#include <QtDBus>
#include <QByteArray>
#include <QHash>
#include <QString>
#include "dbus/dbus_helper.h"

class Secret
{
    Q_PROPERTY(QString session READ getSession)
    Q_PROPERTY(QByteArray parameters READ getParameters)
    Q_PROPERTY(QByteArray value READ getValue)
    Q_PROPERTY(QString contentType READ getContentType)

public:
    Secret();
    Secret(QDBusObjectPath session, QByteArray parameters, QByteArray value, QString contentType);
    Secret(const Secret& other);
    Secret& operator=(const Secret& other);
    ~Secret();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const Secret& secret);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Secret &secret);

    //register Secret with the Qt type system
    static void registerMetaType();

    // property getters
    QString getSession() const;
    QByteArray getParameters() const;
    QByteArray getValue() const;
    QString getContentType() const;

private:
    QDBusObjectPath _session;
    QByteArray _parameters;
    QByteArray _value;
    QString _contentType;
};

typedef QHash<QDBusObjectPath, Secret> DBusSecretHash;

Q_DECLARE_METATYPE(DBusSecretHash)
Q_DECLARE_METATYPE(Secret)

#endif // SECRET_H
