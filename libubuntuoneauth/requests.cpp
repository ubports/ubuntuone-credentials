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

#include "requests.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace UbuntuOne {

RequestInterface::RequestInterface(const QUrl& url)
    : _url(url)
{
}

OAuthTokenRequest::OAuthTokenRequest()
    : RequestInterface(OAUTH_API),
      _email(""), _password(""), _name(""), _otp("")
{
}

OAuthTokenRequest::OAuthTokenRequest(QString email, QString password,
                                     QString name, QString otp)
    : RequestInterface(OAUTH_API),
      _email(email), _password(password), _name(name), _otp(otp)
{
}

QByteArray OAuthTokenRequest::serialize() const
{
    QJsonObject serializer;
    serializer.insert("email", this->email());
    serializer.insert("password", this->password());
    serializer.insert("token_name", this->name());
    if (!this->otp().isEmpty())
        serializer.insert("otp", this->otp());

    QJsonDocument doc(serializer);

    return doc.toJson();
}

AccountRequest::AccountRequest()
    : RequestInterface(ACCOUNTS_API),
      _email(""), _password(""), _name(""),
      _captchaId(""), _captchaSolution(""),
      _createCaptcha(true)
{
}

AccountRequest::AccountRequest(QString email, QString password, QString name,
                               QString captchaId, QString captchaSolution,
                               bool createCaptcha)
    : RequestInterface(ACCOUNTS_API),
      _email(email), _password(password), _name(name),
      _captchaId(captchaId), _captchaSolution(captchaSolution),
      _createCaptcha(createCaptcha)
{
}

QByteArray AccountRequest::serialize() const
{
    QJsonObject data;
    data.insert("email", this->email());
    data.insert("password", this->password());
    data.insert("displayname", this->name());
    data.insert("captcha_id", this->captchaId());
    data.insert("captcha_solution", this->captchaSolution());
    data.insert("create_captcha", this->createCaptcha());

    QJsonDocument doc(data);
    return doc.toJson();
}

PasswordTokenRequest::PasswordTokenRequest()
    : RequestInterface(PASSWORD_API), _email("")
{
}

PasswordTokenRequest::PasswordTokenRequest(QString email)
    : RequestInterface(PASSWORD_API), _email(email)
{
}

QByteArray PasswordTokenRequest::serialize() const
{
    QJsonObject data;
    data.insert("email", this->email());

    QJsonDocument doc(data);
    return doc.toJson();
}

} /* end UbuntuOne namespace */
