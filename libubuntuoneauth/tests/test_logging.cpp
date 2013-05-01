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
#include <QString>

#include <logging.h>

#include "test_logging.h"

using namespace UbuntuOne;

TestAuthLogger::TestAuthLogger()
    : test_cache_path(QDir::currentPath() + "/_test_temp")
{
}

void TestAuthLogger::testLogPath()
{
    QString expected = this->test_cache_path + "/ubuntuone/log";
    QByteArray old_home = qgetenv("XDG_CACHE_HOME");

    qputenv("XDG_CACHE_HOME", this->test_cache_path.toUtf8());

    QCOMPARE(AuthLogger::getLogDirectory(), expected);

    qputenv("XDG_CACHE_HOME", old_home);
}

void TestAuthLogger::testMessageTypeString()
{
    QCOMPARE(AuthLogger::getMessageTypeString(QtDebugMsg),
             QStringLiteral("DEBUG"));
    QCOMPARE(AuthLogger::getMessageTypeString(QtWarningMsg),
             QStringLiteral("WARNING"));
    QCOMPARE(AuthLogger::getMessageTypeString(QtCriticalMsg),
             QStringLiteral("CRITICAL"));
    QCOMPARE(AuthLogger::getMessageTypeString(QtFatalMsg),
             QStringLiteral("FATAL"));
    QCOMPARE(AuthLogger::getMessageTypeString(QtSystemMsg),
             QStringLiteral("CRITICAL"));
}

void TestAuthLogger::testLogLevel()
{
    QCOMPARE(AuthLogger::setLogLevel(QtDebugMsg), false);
    AuthLogger::setupLogging();
    QCOMPARE(AuthLogger::setLogLevel(QtDebugMsg), true);
    AuthLogger::stopLogging();
}
