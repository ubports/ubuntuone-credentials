#include <QtTest/QtTest>

#include "sso_api/test_requests.h"
#include "sso_api/test_responses.h"

int main()
{

    /* Request tests */
    TestOAuthTokenRequests test_oauth_token_requests;
    TestPasswordTokenRequests test_password_token_requests;
    TestAccountRequests test_account_requests;

    /* Response tests */
    TestErrorResponses test_error_responses;
    TestOAuthTokenResponses test_oauth_token_responses;
    TestPasswordTokenResponses test_password_token_responses;

    int result = 0;

    result += QTest::qExec(&test_oauth_token_requests);
    result += QTest::qExec(&test_password_token_requests);
    result += QTest::qExec(&test_account_requests);
    result += QTest::qExec(&test_error_responses);
    result += QTest::qExec(&test_oauth_token_responses);
    result += QTest::qExec(&test_password_token_responses);

    return result;
}

