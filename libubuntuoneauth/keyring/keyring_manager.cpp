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
#include "keyring_manager.h"

KeyringManager::KeyringManager(QObject *parent) :
    QObject(parent),
  _conn(QDBusConnection::sessionBus())
{
    // create the keyring that will be used to store and retrieve the different
    // tokens
    _keyring = new keyring::Keyring(_conn);
    this->connect(_keyring, SIGNAL(sessionOpened()), this, SLOT(sessionDetected()));
    this->connect(_keyring, SIGNAL(credentialsFound(QString,QString,QString,bool)), this, SLOT(credentialsFound(QString,QString,QString)));
}

void KeyringManager::start()
{
    _keyring->openSession();
}

void KeyringManager::sessionDetected()
{
}

void KeyringManager::credentialsFound(QString id, QString token, QString secret)
{
}
