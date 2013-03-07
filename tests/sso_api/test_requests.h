#ifndef _TEST_REQUESTS_H_
#define _TEST_REQUESTS_H_

#include <QtTest/QtTest>

class TestOAuthTokenRequests : public QObject
{
    Q_OBJECT
private slots:
    void testDefaults();
};

#endif /* _TEST_REQUESTS_H_ */
