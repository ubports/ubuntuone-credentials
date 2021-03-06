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

#include "test_responses.h"

#include "responses.h"
#include "errormessages.h"

using namespace UbuntuOne;

TestErrorResponses::TestErrorResponses()
    : test_httpStatus(123),
      test_httpReason(QUuid::createUuid().toString()),
      test_code(QUuid::createUuid().toString()),
      test_message(QUuid::createUuid().toString()),
      test_imageUrl(QUuid::createUuid().toString()),
      test_captchaId(QUuid::createUuid().toString()),
      test_captchaMessage(QUuid::createUuid().toString()),
      test_email(QUuid::createUuid().toString())
{
}

void TestErrorResponses::testDefaults()
{
    ErrorResponse response;

    QString empty("");

    QCOMPARE(response.httpStatus(), -1);
    QCOMPARE(response.httpReason(), empty);
    QCOMPARE(response.code(), -1);
    QCOMPARE(response.message(), empty);
    QCOMPARE(response.imageUrl(), empty);
    QCOMPARE(response.captchaId(), empty);
    QCOMPARE(response.captchaMessage(), empty);
    QCOMPARE(response.email(), empty);
}

void TestErrorResponses::testValues()
{
    ErrorResponse response;

    response.httpStatus(test_httpStatus);
    response.httpReason(test_httpReason);
    response.code(test_code);
    response.message(test_message);
    response.imageUrl(test_imageUrl);
    response.captchaId(test_captchaId);
    response.captchaMessage(test_captchaMessage);
    response.email(test_email);

    QCOMPARE(response.httpStatus(), test_httpStatus);
    QCOMPARE(response.httpReason(), test_httpReason);
    QCOMPARE(response.code(), -1);
    QCOMPARE(response.message(), test_message);
    QCOMPARE(response.imageUrl(), test_imageUrl);
    QCOMPARE(response.captchaId(), test_captchaId);
    QCOMPARE(response.captchaMessage(), test_captchaMessage);
    QCOMPARE(response.email(), test_email);
}

void TestErrorResponses::testErrorMessage()
{
    ErrorResponse response;

    response.code(CAPTCHA_REQUIRED);
    QCOMPARE(response.code(), int(ErrorCodes::CODE_CAPTCHA_REQUIRED));
}

TestOAuthTokenResponses::TestOAuthTokenResponses()
    : test_token_name(QUuid::createUuid().toString()),
      test_token_secret(QUuid::createUuid().toString()),
      test_token_key(QUuid::createUuid().toString()),
      test_date_created(QUuid::createUuid().toString()),
      test_date_updated(QUuid::createUuid().toString()),
      test_consumer_secret(QUuid::createUuid().toString()),
      test_consumer_key(QUuid::createUuid().toString()),
      test_href(QUuid::createUuid().toString())
{
}

void TestOAuthTokenResponses::testDefaults()
{
    OAuthTokenResponse response;

    QString empty("");

    QCOMPARE(response.token_name(), empty);
    QCOMPARE(response.token_secret(), empty);
    QCOMPARE(response.token_key(), empty);
    QCOMPARE(response.date_created(), empty);
    QCOMPARE(response.date_updated(), empty);
    QCOMPARE(response.consumer_secret(), empty);
    QCOMPARE(response.consumer_key(), empty);
    QCOMPARE(response.href(), empty);
}

void TestOAuthTokenResponses::testValues()
{
    OAuthTokenResponse response;

    response.token_name(test_token_name);
    response.token_secret(test_token_secret);
    response.token_key(test_token_key);
    response.date_created(test_date_created);
    response.date_updated(test_date_updated);
    response.consumer_secret(test_consumer_secret);
    response.consumer_key(test_consumer_key);
    response.href(test_href);

    QCOMPARE(response.token_name(), test_token_name);
    QCOMPARE(response.token_secret(), test_token_secret);
    QCOMPARE(response.token_key(), test_token_key);
    QCOMPARE(response.date_created(), test_date_created);
    QCOMPARE(response.date_updated(), test_date_updated);
    QCOMPARE(response.consumer_secret(), test_consumer_secret);
    QCOMPARE(response.consumer_key(), test_consumer_key);
    QCOMPARE(response.href(), test_href);
}

TestPasswordTokenResponses::TestPasswordTokenResponses()
    : test_email(QUuid::createUuid().toString())
{
}

void TestPasswordTokenResponses::testDefaults()
{
    PasswordTokenResponse response;

    QString empty("");

    QCOMPARE(response.email(), empty);
}

void TestPasswordTokenResponses::testValues()
{
    PasswordTokenResponse response;

    response.email(test_email);

    QCOMPARE(response.email(), test_email);
}

TestAccountResponses::TestAccountResponses()
    : test_status(QUuid::createUuid().toString()),
      test_openid(QUuid::createUuid().toString()),
      test_displayname(QUuid::createUuid().toString()),
      test_href(QUuid::createUuid().toString()),
      test_email(QUuid::createUuid().toString())
{
}

void TestAccountResponses::testDefaults()
{
    AccountResponse response;

    QString empty("");

    QCOMPARE(response.status(), empty);
    QCOMPARE(response.openid(), empty);
    QCOMPARE(response.displayname(), empty);
    QCOMPARE(response.href(), empty);
    QCOMPARE(response.email(), empty);
    /*QCOMPARE(response.emails(), empty);*/
}

void TestAccountResponses::testValues()
{
    AccountResponse response;

    response.status(test_status);
    response.openid(test_openid);
    response.displayname(test_displayname);
    response.href(test_href);
    response.email(test_email);
    /*response.emails(test_emails);*/

    QCOMPARE(response.status(), test_status);
    QCOMPARE(response.openid(), test_openid);
    QCOMPARE(response.displayname(), test_displayname);
    QCOMPARE(response.href(), test_href);
    QCOMPARE(response.email(), test_email);
    /*QCOMPARE(response.emails(), test_emails);*/
}
