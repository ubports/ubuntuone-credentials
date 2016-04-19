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
#ifndef _U1_TOKEN_H_
#define _U1_TOKEN_H_

#include <QDateTime>
#include <QHash>
#include <QString>
#include <QUrl>

#include "common.h"

namespace UbuntuOne {

#define TOKEN_ID "Ubuntu One"
#define TOKEN_SEP " @ "
#define TOKEN_SEP_REPLACEMENT " AT "

    class U1_EXPORT Token
    {
    public:
        Token() {};
        Token(QString token_key, QString token_secret,
              QString consumer_key, QString consumer_secret);
        Token(QString token_key, QString token_secret,
              QString consumer_key, QString consumer_secret,
              QString created_date, QString updated_date);

        QString toQuery();
        bool isValid() const;

        QDateTime created() const;
        QDateTime updated() const;

        QDateTime getServerTimestamp() const;
        QUrl addOAuthTimestamp(const QString url) const;
        QString signUrl(const QString url, const QString method, bool asQuery = false) const;

        static Token *fromQuery(const QString query);
        static QString buildTokenName();

        static QString dateStringToISO(const QString date);

        QString consumerKey() const;

    private:
        QHash<QString, QString> _tokenHash;
    };

} /* namespace UbuntuOne */

#endif /* _U1_TOKEN_H_ */
