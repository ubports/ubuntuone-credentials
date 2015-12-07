/*
 * Copyright 2013-2015 Canonical Ltd.
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
#include <QDir>
#include <QHostInfo>
#include <QUuid>

#include <token.h>

#include "test_token.h"

using namespace UbuntuOne;


void TestToken::cleanupTestCase()
{
    qputenv("SSO_AUTH_BASE_URL", old_base_url.toUtf8().data());
    process->close();
    process->deleteLater();
}

TestToken::TestToken()
    : test_hostname(QHostInfo::localHostName()),
      old_base_url("")
{
}

void TestToken::testEmptyToken()
{
    Token *token = new Token();
    QVERIFY(!token->isValid());
    delete token;
}

void TestToken::testTokenArgs()
{
    Token *token = new Token("a", "b", "c", "d");
    QVERIFY(token->isValid());
    delete token;
}

void TestConsumerKey()
{
    Token token("a", "b", "c", "d");
    QString expected("c");
    QCOMPARE(token.consumerKey(), expected);
}

void TestConsumerKeyOfEmptyToken()
{
    Token token;
    QVERIFY(!token.isValid());
    QCOMPARE(token.consumerKey(), QString(""));
}

void TestToken::testTokenCopy()
{
    Token *old_token = new Token("a", "b", "c", "d");
    Token new_token = Token(*old_token);
    QVERIFY(new_token.isValid());
    delete old_token;
}

void TestToken::testToQuery()
{
    Token *token = new Token("token", "t_secret", "consumer", "c_secret");
    QString query = token->toQuery();
    QString expected("consumer_key=consumer&consumer_secret=c_secret&name=Ubuntu+One+%40+" + test_hostname + "&token=token&token_secret=t_secret");
    QCOMPARE(query, expected);
    delete token;
}

void TestToken::testFromQuery()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s");
    QVERIFY(token->isValid());
    delete token;
}

void TestToken::testTokenName()
{
    QString expected = QStringLiteral(TOKEN_ID) + " @ " + test_hostname;
    QCOMPARE(Token::buildTokenName(), expected);
}

void TestToken::testSignUrlEmpty()
{
    Token *token = new Token("a", "b", "c", "d");
    QCOMPARE(token->signUrl(QStringLiteral(""), QStringLiteral("GET")),
             QStringLiteral(""));
    delete token;
}

void TestToken::testSignUrl()
{
    Token *token = new Token("a", "b", "c", "d");
    auto result = token->signUrl(QStringLiteral("https://login.ubuntu.com"),
                                 QStringLiteral("GET"));
    QVERIFY(result.startsWith("OAuth oauth_consumer_key"));
    delete token;
}

void TestToken::testGetServerTimestamp()
{
    Token *token = new Token("a", "b", "c", "d");
    auto result = token->getServerTimestamp();
    delete token;

    QCOMPARE(result.toTime_t(), (uint)time(NULL));
}

void TestToken::testGetServerTimestampMuchEarlier()
{
    old_base_url = qgetenv("SSO_AUTH_BASE_URL");
    qputenv("SSO_AUTH_BASE_URL", "http://localhost:8000/muchearlier");
    qputenv("U1_TEST_TIMESTAMP", "Thu, 01 Jan 1970 04:32:16 GMT");

    auto _app = new QCoreApplication(argc, argv);

    QDateTime result;
    QTimer::singleShot(0, [this, &result, &_app](){
            process = new QProcess(this);
            QSignalSpy spy(process, SIGNAL(started()));
            QString program = "python3";
            QString script = QDir::currentPath() + "/mock_sso_server.py";
            process->start(program, QStringList() << script);
            QTRY_COMPARE(spy.count(), 1);

            // Wait for server to start
            QTimer timer;
            QSignalSpy spy2(&timer, SIGNAL(timeout()));
            timer.setInterval(2000);
            timer.setSingleShot(true);
            timer.start();
            QTRY_COMPARE(spy2.count(), 1);

            Token *token = new Token("a", "b", "c", "d");
            result = token->getServerTimestamp();
            delete token;

            _app->quit();
        });

    _app->exec();

    QCOMPARE(result.toTime_t(), (uint)864016336);
}

void TestToken::testGetServerTimestampMuchLater()
{
    old_base_url = qgetenv("SSO_AUTH_BASE_URL");
    qputenv("SSO_AUTH_BASE_URL", "http://localhost:8000/muchlater");
    qputenv("U1_TEST_TIMESTAMP", "Mon, 18 Jan 2038 22:14:07 GMT");

    auto _app = new QCoreApplication(argc, argv);

    QDateTime result;
    QTimer::singleShot(0, [this, &result, &_app](){
            process = new QProcess(this);
            QSignalSpy spy(process, SIGNAL(started()));
            QString program = "python3";
            QString script = QDir::currentPath() + "/mock_sso_server.py";
            process->start(program, QStringList() << script);
            QTRY_COMPARE(spy.count(), 1);

            // Wait for server to start
            QTimer timer;
            QSignalSpy spy2(&timer, SIGNAL(timeout()));
            timer.setInterval(2000);
            timer.setSingleShot(true);
            timer.start();
            QTRY_COMPARE(spy2.count(), 1);

            Token *token = new Token("a", "b", "c", "d");
            result = token->getServerTimestamp();
            delete token;

            _app->quit();
        });

    _app->exec();

    QCOMPARE(result.toTime_t(), (uint)1283465647);
}

void TestToken::testTimesCached()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s&updated=2014-08-11+18%3A40%3A20.777777&created=2014-08-11+18%3A40%3A20.777777");
    QVERIFY(token->isValid());
    QString query = token->toQuery();
    // Dates are converted to ISO when parsed.
    QVERIFY(query.contains("updated=2014-08-11T18:40:20Z"));
    QVERIFY(query.contains("created=2014-08-11T18:40:20Z"));
    delete token;
}

void TestToken::testCreatedParsed()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s&updated=2014-08-11+18%3A40%3A20.777777&created=2014-08-11+18%3A40%3A20.777777");
    unsigned int expected = 1407782420;
    QVERIFY(token->created().isValid());
    QCOMPARE(token->created().toTime_t(), expected);
    delete token;
}

void TestToken::testUpdatedParsed()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s&updated=2014-08-11+18%3A40%3A20.777777&created=2014-08-11+18%3A40%3A20.777777");
    unsigned int expected = 1407782420;
    QVERIFY(token->updated().isValid());
    QCOMPARE(token->updated().toTime_t(), expected);
    delete token;
}

void TestToken::testCreatedMissing()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s");
    QVERIFY(token->isValid());
    QVERIFY(!token->created().isValid());
    QCOMPARE(token->created().toTime_t(), UINT_MAX);
    delete token;
}

void TestToken::testUpdatedMissing()
{
    Token *token = Token::fromQuery("consumer_key=c&consumer_secret=c_s&name=Name+%40+hostname&token=t&token_secret=t_s");
    QVERIFY(token->isValid());
    QVERIFY(!token->updated().isValid());
    QCOMPARE(token->updated().toTime_t(), UINT_MAX);
    delete token;
}

void TestToken::testNewWithDates()
{
    Token *token = new Token("token", "t_secret", "consumer", "c_secret",
                             "2014-08-11+18%3A40%3A20.777777",
                             "2014-08-11T18:40:20Z");
    unsigned int expected = 1407782420;

    QVERIFY(token->isValid());
    QVERIFY(token->created().isValid());
    QCOMPARE(token->created().toTime_t(), expected);
    QVERIFY(token->updated().isValid());
    QCOMPARE(token->updated().toTime_t(), expected);
    delete token;
}

void TestToken::testSSODateToISO()
{
    QString in_date{"2014-08-11+18%3A40%3A20.777777"};
    QString expected{"2014-08-11T18:40:20Z"};
    QCOMPARE(Token::dateStringToISO(in_date), expected);
}

void TestToken::testISODateToISO()
{
    QString in_date{"2014-08-11T18:40:20Z"};
    QCOMPARE(Token::dateStringToISO(in_date), in_date);
}
