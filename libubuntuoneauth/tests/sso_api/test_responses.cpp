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

#include "sso_api/responses.h"

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
    SSO::ErrorResponse response;

    QString empty("");

    QCOMPARE(response.httpStatus(), -1);
    QCOMPARE(response.httpReason(), empty);
    QCOMPARE(response.code(), empty);
    QCOMPARE(response.message(), empty);
    QCOMPARE(response.imageUrl(), empty);
    QCOMPARE(response.captchaId(), empty);
    QCOMPARE(response.captchaMessage(), empty);
    QCOMPARE(response.email(), empty);
}

void TestErrorResponses::testValues()
{
    SSO::ErrorResponse response;

    response.httpStatus(this->test_httpStatus);
    response.httpReason(this->test_httpReason);
    response.code(this->test_code);
    response.message(this->test_message);
    response.imageUrl(this->test_imageUrl);
    response.captchaId(this->test_captchaId);
    response.captchaMessage(this->test_captchaMessage);
    response.email(this->test_email);

    QCOMPARE(response.httpStatus(), this->test_httpStatus);
    QCOMPARE(response.httpReason(), this->test_httpReason);
    QCOMPARE(response.code(), this->test_code);
    QCOMPARE(response.message(), this->test_message);
    QCOMPARE(response.imageUrl(), this->test_imageUrl);
    QCOMPARE(response.captchaId(), this->test_captchaId);
    QCOMPARE(response.captchaMessage(), this->test_captchaMessage);
    QCOMPARE(response.email(), this->test_email);
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
    SSO::OAuthTokenResponse response;

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
    SSO::OAuthTokenResponse response;

    response.token_name(this->test_token_name);
    response.token_secret(this->test_token_secret);
    response.token_key(this->test_token_key);
    response.date_created(this->test_date_created);
    response.date_updated(this->test_date_updated);
    response.consumer_secret(this->test_consumer_secret);
    response.consumer_key(this->test_consumer_key);
    response.href(this->test_href);

    QCOMPARE(response.token_name(), this->test_token_name);
    QCOMPARE(response.token_secret(), this->test_token_secret);
    QCOMPARE(response.token_key(), this->test_token_key);
    QCOMPARE(response.date_created(), this->test_date_created);
    QCOMPARE(response.date_updated(), this->test_date_updated);
    QCOMPARE(response.consumer_secret(), this->test_consumer_secret);
    QCOMPARE(response.consumer_key(), this->test_consumer_key);
    QCOMPARE(response.href(), this->test_href);
}

TestPasswordTokenResponses::TestPasswordTokenResponses()
    : test_email(QUuid::createUuid().toString())
{
}

void TestPasswordTokenResponses::testDefaults()
{
    SSO::PasswordTokenResponse response;

    QString empty("");

    QCOMPARE(response.email(), empty);
}

void TestPasswordTokenResponses::testValues()
{
    SSO::PasswordTokenResponse response;

    response.email(this->test_email);

    QCOMPARE(response.email(), this->test_email);
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
    SSO::AccountResponse response;

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
    SSO::AccountResponse response;

    response.status(this->test_status);
    response.openid(this->test_openid);
    response.displayname(this->test_displayname);
    response.href(this->test_href);
    response.email(this->test_email);
    /*response.emails(this->test_emails);*/

    QCOMPARE(response.status(), this->test_status);
    QCOMPARE(response.openid(), this->test_openid);
    QCOMPARE(response.displayname(), this->test_displayname);
    QCOMPARE(response.href(), this->test_href);
    QCOMPARE(response.email(), this->test_email);
    /*QCOMPARE(response.emails(), this->test_emails);*/
}
