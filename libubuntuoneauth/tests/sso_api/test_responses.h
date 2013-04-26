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
#ifndef _TEST_RESPONSES_H_
#define _TEST_RESPONSES_H_

#include <QtTest/QtTest>

class TestErrorResponses: public QObject
{
    Q_OBJECT
public:
    TestErrorResponses();

private:
    int test_httpStatus;
    QString test_httpReason, test_code, test_message, test_imageUrl,
            test_captchaId, test_captchaMessage, test_email;

private slots:
    void testDefaults();
    void testValues();
};

class TestOAuthTokenResponses : public QObject
{
    Q_OBJECT
public:
    TestOAuthTokenResponses();

private:
    QString test_token_name, test_token_secret, test_token_key,
            test_date_created, test_date_updated, test_consumer_secret,
            test_consumer_key, test_href;

private slots:
    void testDefaults();
    void testValues();
};

class TestPasswordTokenResponses : public QObject
{
    Q_OBJECT
public:
    TestPasswordTokenResponses();

private:
    QString test_email;

private slots:
    void testDefaults();
    void testValues();
};

class TestAccountResponses : public QObject
{
    Q_OBJECT
public:
    TestAccountResponses();

private:
    QString test_status, test_openid, test_displayname, test_href,
            test_email;
    QStringList test_emails;

private slots:
    void testDefaults();
    void testValues();
};

#endif /* _TEST_RESPONSES_H_ */
