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
#include <QByteArray>
#include <QtTest/QtTest>

#include "test-plugin.h"


int main()
{
    QByteArray fake_home = QDir::currentPath().toUtf8() + "/_test_temp";
    /* Need to alter some environment variables to avoid contamination. */
    qputenv("HOME", fake_home);
    qputenv("XDG_CACHE_HOME", fake_home);
    qputenv("XDG_DATA_HOME", fake_home);

    /* The tests */
    TestPlugin test_plugin;

    int result = 0;

    result += QTest::qExec(&test_plugin);

    return result;
}
