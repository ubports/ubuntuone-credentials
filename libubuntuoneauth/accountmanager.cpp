/*
 * Copyright 2016 Canonical Ltd.
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

#include "accountmanager.h"

namespace UbuntuOne {

AccountManager *AccountManager::m_instance = 0;

AccountManager::AccountManager(QObject *parent):
    Accounts::Manager(parent)
{
}

AccountManager::~AccountManager()
{
    m_instance = 0;
}

AccountManager *AccountManager::instance()
{
    if (!m_instance) {
        m_instance = new AccountManager;
    }

    return m_instance;
}

} // namespace
