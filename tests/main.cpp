#include <QtTest/QtTest>

#include "sso_api/test_requests.h"

int main()
{

    TestOAuthTokenRequests test_oauth_token_requests;
    TestAccountRequests test_account_requests;

    int result = 0;

    result += QTest::qExec(&test_oauth_token_requests);
    result += QTest::qExec(&test_account_requests);

    return result;
}

