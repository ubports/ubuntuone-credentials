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
#ifndef _U1_TOKEN_H_
#define _U1_TOKEN_H_

#include <QHash>
#include <QString>


namespace UbuntuOne {

#define TOKEN_ID "Ubuntu One"
#define TOKEN_SEP " @ "
#define TOKEN_SEP_REPLACEMENT " AT "

    class Token
    {
    public:
        Token();
        Token(QString token_key, QString token_secret,
              QString consumer_key, QString consumer_secret);
        Token(const Token& token);

        QString toQuery();
        bool isValid();

        static Token *fromQuery(const QString query);
        static QString buildTokenName();

    private:
        QHash<QString, QString> _tokenHash;
    };

} /* namespace UbuntuOne */

#endif /* _U1_KEYRING_H_ */
