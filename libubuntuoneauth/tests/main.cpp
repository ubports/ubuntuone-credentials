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

#include "test_logging.h"
#include "keyring/test_keyring.h"
#include "sso_api/test_requests.h"
#include "sso_api/test_responses.h"

int main()
{
    QByteArray fake_home = QDir::currentPath().toUtf8() + "/_test_temp";
    /* Need to alter some environment variables to avoid contamination. */
    qputenv("HOME", fake_home);
    qputenv("XDG_CACHE_HOME", fake_home);
    qputenv("XDG_DATA_HOME", fake_home);

    /* Logging tests */
    TestAuthLogger test_auth_logger;

    /* Keyring tests */
    TestKeyring test_keyring;

    /* Request tests */
    TestOAuthTokenRequests test_oauth_token_requests;
    TestPasswordTokenRequests test_password_token_requests;
    TestAccountRequests test_account_requests;

    /* Response tests */
    TestErrorResponses test_error_responses;
    TestOAuthTokenResponses test_oauth_token_responses;
    TestPasswordTokenResponses test_password_token_responses;

    int result = 0;

    result += QTest::qExec(&test_auth_logger);
    result += QTest::qExec(&test_keyring);
    result += QTest::qExec(&test_oauth_token_requests);
    result += QTest::qExec(&test_password_token_requests);
    result += QTest::qExec(&test_account_requests);
    result += QTest::qExec(&test_error_responses);
    result += QTest::qExec(&test_oauth_token_responses);
    result += QTest::qExec(&test_password_token_responses);

    return result;
}

