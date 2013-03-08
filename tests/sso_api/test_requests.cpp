#include <QUuid>

#include "test_requests.h"

#include "lib/sso_api/requests.h"

TestOAuthTokenRequests::TestOAuthTokenRequests()
    : test_email(QUuid::createUuid().toString()),
      test_password(QUuid::createUuid().toString()),
      test_name(QUuid::createUuid().toString()),
      test_otp(QUuid::createUuid().toString())
{
}

void TestOAuthTokenRequests::testDefaults()
{
    SSO::OAuthTokenRequest request;

    QString empty("");

    QCOMPARE(request.email(), empty);
    QCOMPARE(request.password(), empty);
    QCOMPARE(request.name(), empty);
    QCOMPARE(request.otp(), empty);
}

void TestOAuthTokenRequests::testValues()
{
    SSO::OAuthTokenRequest request;

    request.email(this->test_email);
    request.password(this->test_password);
    request.name(this->test_name);
    request.otp(this->test_otp);

    QCOMPARE(request.email(), this->test_email);
    QCOMPARE(request.password(), this->test_password);
    QCOMPARE(request.name(), this->test_name);
    QCOMPARE(request.otp(), this->test_otp);

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
    SSO::AccountRequest request;

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
    SSO::AccountRequest request;
    bool create = false;

    request.email(this->test_email);
    request.password(this->test_password);
    request.name(this->test_name);
    request.captchaId(this->test_captchaId);
    request.captchaSolution(this->test_captchaSolution);
    request.createCaptcha(create);

    QCOMPARE(request.email(), this->test_email);
    QCOMPARE(request.password(), this->test_password);
    QCOMPARE(request.name(), this->test_name);
    QCOMPARE(request.captchaId(), this->test_captchaId);
    QCOMPARE(request.captchaSolution(), this->test_captchaSolution);
    QCOMPARE(request.createCaptcha(), create);
}


