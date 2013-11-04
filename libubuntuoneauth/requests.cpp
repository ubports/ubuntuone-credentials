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

OAuthTokenRequest::OAuthTokenRequest(QString base_url,
                                     QString email, QString password,
                                     QString name, QString otp)
    : RequestInterface(base_url + OAUTH_PATH),
      _email(email), _password(password), _name(name), _otp(otp)
{
}

QByteArray OAuthTokenRequest::serialize() const
{
    QJsonObject serializer;
    serializer.insert("email", email());
    serializer.insert("password", password());
    serializer.insert("token_name", name());
    if (!otp().isEmpty())
        serializer.insert("otp", otp());

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

AccountRequest::AccountRequest(QString base_url,
                               QString email, QString password, QString name,
                               QString captchaId, QString captchaSolution,
                               bool createCaptcha)
    : RequestInterface(base_url + ACCOUNTS_PATH),
      _email(email), _password(password), _name(name),
      _captchaId(captchaId), _captchaSolution(captchaSolution),
      _createCaptcha(createCaptcha)
{
}

AccountRequest::AccountRequest(QString base_url,
                               QString email, QString password, QString name)
    : RequestInterface(base_url + ACCOUNTS_PATH),
      _email(email), _password(password), _name(name),
      _captchaId(QString()), _captchaSolution(QString()),
      _createCaptcha(false)
{
}

QByteArray AccountRequest::serialize() const
{
    QJsonObject data;
    data.insert("email", email());
    data.insert("password", password());
    data.insert("displayname", name());
    data.insert("captcha_id", captchaId());
    data.insert("captcha_solution", captchaSolution());
    data.insert("create_captcha", createCaptcha());

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

PasswordTokenRequest::PasswordTokenRequest(QString base_url, QString email)
    : RequestInterface(base_url + PASSWORD_PATH), _email(email)
{
}

QByteArray PasswordTokenRequest::serialize() const
{
    QJsonObject data;
    data.insert("email", email());

    QJsonDocument doc(data);
    return doc.toJson();
}

} /* end UbuntuOne namespace */
