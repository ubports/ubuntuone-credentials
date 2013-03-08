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
