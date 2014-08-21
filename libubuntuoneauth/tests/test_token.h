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
#ifndef _TEST_TOKEN_H_
#define _TEST_TOKEN_H_

#include <QtTest/QtTest>

class TestToken: public QObject
{
    Q_OBJECT
 public:
    TestToken();

 private:
    QString test_hostname;

    private slots:
        void testEmptyToken();
        void testTokenArgs();
        void testTokenCopy();

        void testToQuery();

        void testFromQuery();
        void testTokenName();

        void testSignUrlEmpty();

        void testTimesCached();
        void testCreatedParsed();
        void testUpdatedParsed();
};

#endif /* _TEST_TOKEN_H_ */
