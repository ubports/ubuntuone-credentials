/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or
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

#ifndef _ACL_UPDATER_H_
#define _ACL_UPDATER_H_

#include <QObject>

namespace SignOn {
    class Error;
    class Identity;
    class IdentityInfo;
};

class AclUpdater : public QObject
{
    Q_OBJECT

public:
    AclUpdater(quint32 credentialsId, QObject *parent = 0);

Q_SIGNALS:
    void finished();

private Q_SLOTS:
    void onInfoReady(const SignOn::IdentityInfo &info);
    void onError(const SignOn::Error &error);

private:
    SignOn::Identity *identity;
};

#endif // _ACL_UPDATER_H_
