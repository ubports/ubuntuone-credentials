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

#ifndef KEYRING_MANAGER_H
#define KEYRING_MANAGER_H

#include <QObject>
#include <QDBusConnection>
#include "keyring.h"

class KeyringManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyringManager(QObject *parent = 0);

    void start();

signals:

public slots:
    void sessionDetected();
    void credentialsFound(QString id, QString token, QString secret);

private:
    QDBusConnection _conn;
    keyring::Keyring* _keyring;
};

#endif // KEYRING_MANAGER_H
