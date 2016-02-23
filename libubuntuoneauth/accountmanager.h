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
#ifndef _U1_ACCOUNTMANAGER_H_
#define _U1_ACCOUNTMANAGER_H_

#include <Accounts/Manager>
#include <QObject>

namespace UbuntuOne {

class AccountManager : public Accounts::Manager
{
    Q_OBJECT

public:
    static AccountManager *instance();
    ~AccountManager();

protected:
    explicit AccountManager(QObject *parent = 0);

private:
    static AccountManager *m_instance;
};

} /* namespace UbuntuOne */

#endif /* _U1_ACCOUNTMANAGER_H_ */
