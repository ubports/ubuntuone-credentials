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
    QString expected("consumer_key=consumer&consumer_secret=c_secret&name=Ubuntu+One+%40+" + this->test_hostname + "&token=token&token_secret=t_secret");
    QCOMPARE(query.toUtf8().data(), expected.toUtf8().data());
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
    QString expected = QStringLiteral(TOKEN_ID) + " @ " + this->test_hostname;
    QCOMPARE(Token::buildTokenName(), expected);
}
