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
#include <QHostInfo>
#include <QUuid>

#include <token.h>

#include "test_token.h"

using namespace UbuntuOne;


TestToken::TestToken()
    : test_hostname(QHostInfo::localHostName())
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
    QString expected("c");
    QCOMPARE(token->consumerKey(), expected);
    delete token;
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
