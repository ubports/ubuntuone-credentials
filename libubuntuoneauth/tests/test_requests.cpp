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

#include <QUuid>

#include "test_requests.h"

#include "requests.h"

using namespace UbuntuOne;


TestOAuthTokenRequests::TestOAuthTokenRequests()
    : test_email(QUuid::createUuid().toString()),
      test_password(QUuid::createUuid().toString()),
      test_name(QUuid::createUuid().toString()),
      test_otp(QUuid::createUuid().toString())
{
}

void TestOAuthTokenRequests::testDefaults()
{
    OAuthTokenRequest request;

    QString empty("");

    QCOMPARE(request.email(), empty);
    QCOMPARE(request.password(), empty);
    QCOMPARE(request.name(), empty);
    QCOMPARE(request.otp(), empty);
}

void TestOAuthTokenRequests::testValues()
{
    OAuthTokenRequest request;

    request.email(test_email);
    request.password(test_password);
    request.name(test_name);
    request.otp(test_otp);

    QCOMPARE(request.email(), test_email);
    QCOMPARE(request.password(), test_password);
    QCOMPARE(request.name(), test_name);
    QCOMPARE(request.otp(), test_otp);

}

TestAccountRequests::TestAccountRequests()
    : test_email(QUuid::createUuid().toString()),
      test_password(QUuid::createUuid().toString()),
      test_name(QUuid::createUuid().toString()),
      test_captchaId(QUuid::createUuid().toString()),
      test_captchaSolution(QUuid::createUuid().toString())
{
}

void TestAccountRequests::testDefaults()
{
    AccountRequest request;

    QString empty("");

    QCOMPARE(request.email(), empty);
    QCOMPARE(request.password(), empty);
    QCOMPARE(request.name(), empty);
    QCOMPARE(request.captchaId(), empty);
    QCOMPARE(request.captchaSolution(), empty);
    QCOMPARE(request.createCaptcha(), true);
}

void TestAccountRequests::testValues()
{
    AccountRequest request;
    bool create = false;

    request.email(test_email);
    request.password(test_password);
    request.name(test_name);
    request.captchaId(test_captchaId);
    request.captchaSolution(test_captchaSolution);
    request.createCaptcha(create);

    QCOMPARE(request.email(), test_email);
    QCOMPARE(request.password(), test_password);
    QCOMPARE(request.name(), test_name);
    QCOMPARE(request.captchaId(), test_captchaId);
    QCOMPARE(request.captchaSolution(), test_captchaSolution);
    QCOMPARE(request.createCaptcha(), create);
}

TestPasswordTokenRequests::TestPasswordTokenRequests()
    : test_email(QUuid::createUuid().toString())
{
}

void TestPasswordTokenRequests::testDefaults()
{
    PasswordTokenRequest request;

    QString empty("");

    QCOMPARE(request.email(), empty);
}

void TestPasswordTokenRequests::testValues()
{
    PasswordTokenRequest request;

    request.email(test_email);

    QCOMPARE(request.email(), test_email);
}

