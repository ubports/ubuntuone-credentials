#ifndef _TEST_RESPONSES_H_
#define _TEST_RESPONSES_H_

#include <QtTest/QtTest>

class TestErrorResponses: public QObject
{
    Q_OBJECT
public:
    TestErrorResponses();
    ~TestErrorResponses() {};

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

/*
class TestPasswordTokenResponse : public QObject
{
    Q_OBJECT
public:
    TestPasswordTokenResponse();

private:
    QString test_email;

private slots:
    void testDefaults();
    void testValues();
};

class TestAccountResponse : public QObject
{
    Q_OBJECT
public:
    TestAccountResponse();

private:
    QString status, openid, displayname, href, email;
    QStringList emails;

private slots:
    void testDefaults();
    void testValues();
}
*/

#endif /* _TEST_RESPONSES_H_ */
