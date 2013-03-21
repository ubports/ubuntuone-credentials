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

#ifndef _TEST_REQUESTS_H_
#define _TEST_REQUESTS_H_

#include <QtTest/QtTest>

class TestOAuthTokenRequests : public QObject
{
    Q_OBJECT
public:
    TestOAuthTokenRequests();

private:
    QString test_email, test_password, test_name, test_otp;

private slots:
    void testDefaults();
    void testValues();
};

class TestAccountRequests : public QObject
{
    Q_OBJECT
public:
    TestAccountRequests();

private:
    QString test_email, test_password, test_name, test_captchaId,
            test_captchaSolution, test_createCaptcha;

private slots:
    void testDefaults();
    void testValues();
};

class TestPasswordTokenRequests : public QObject
{
    Q_OBJECT
public:
    TestPasswordTokenRequests();

private:
    QString test_email;

private slots:
    void testDefaults();
    void testValues();
};

#endif /* _TEST_REQUESTS_H_ */
