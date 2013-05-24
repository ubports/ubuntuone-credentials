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
#ifndef _TEST_KEYRING_H_
#define _TEST_KEYRING_H_

#include <QtTest/QtTest>

class TestKeyring: public QObject
{
    Q_OBJECT
 public:
    TestKeyring();

 private:
    bool _foundToken;

 private slots:
    void testFindToken();
    void testStoreToken();
    void testDeleteToken();
};

#endif /* _TEST_KEYRING_H_ */

