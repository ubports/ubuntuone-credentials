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

#include "ubuntuone-plugin.h"
#include "ubuntuone-plugin.cpp"
#include "ubuntuonedata.h"

#include "test-plugin.h"

using namespace UbuntuOne;


void TestPlugin::initTestCase()
{
    qRegisterMetaType<SignOn::SessionData>();
    qRegisterMetaType<SignOn::UiSessionData>();
}

void TestPlugin::cleanupTestCase()
{
}

void TestPlugin::init()
{
    m_testPlugin = new SignOnPlugin();
}


void TestPlugin::cleanup()
{
    delete m_testPlugin;
    m_testPlugin = NULL;
}

void TestPlugin::testPluginInit()
{
    QVERIFY(m_testPlugin);
}

void TestPlugin::testPluginType()
{
    QCOMPARE(m_testPlugin->type(), QString("ubuntuone"));
}

void TestPlugin::testPluginMechanisms()
{
    QStringList mechs = m_testPlugin->mechanisms();
    QVERIFY(!mechs.isEmpty());
    QVERIFY(mechs.contains(QString("ubuntuone")));
}
