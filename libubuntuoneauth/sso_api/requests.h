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

#ifndef _REQUESTS_H_
#define _REQUESTS_H_

#include <QString>
#include <QUrl>

namespace UbuntuOne {

static QString API_BASE = "https://login.ubuntu.com/api/v2/";
static QString OAUTH_API = API_BASE + "tokens/oauth";
static QString PASSWORD_API = API_BASE + "tokens/password";
static QString ACCOUNTS_API = API_BASE + "accounts";

class RequestInterface
{
public:
    RequestInterface(const QUrl& url);

    virtual QByteArray serialize() const = 0;
    QUrl url() const { return _url; }

protected:
    QUrl _url;
};

class OAuthTokenRequest : public RequestInterface
{
public:
    OAuthTokenRequest();
    OAuthTokenRequest(QString email, QString password,
                      QString token, QString otp);

    QByteArray serialize() const;

    QString email() const { return _email; }
    QString password() const { return _password; }
    QString name() const { return _name; }
    QString otp() const { return _otp; }

    void email(QString& val) { _email = val; }
    void password(QString& val) { _password = val; }
    void name(QString& val) { _name = val; }
    void otp(QString& val) { _otp = val; }

private:
    QString _email, _password, _name, _otp;
};

class AccountRequest : public RequestInterface
{
public:
    AccountRequest();
    AccountRequest(QString email, QString password, QString name,
                   QString captchaId, QString captchaSolution,
                   bool createCaptcha = true);

    QByteArray serialize() const;

    QString email() const { return _email; }
    QString password() const { return _password; }
    QString name() const { return _name; }
    QString captchaId() const { return _captchaId; }
    QString captchaSolution() const { return _captchaSolution; }
    bool createCaptcha() const { return _createCaptcha; }

    void email(QString& val) { _email = val; }
    void password(QString& val) { _password = val; }
    void name(QString& val) { _name = val; }
    void captchaId(QString& val) { _captchaId = val; }
    void captchaSolution(QString& val) { _captchaSolution = val; }
    void createCaptcha(bool& val) { _createCaptcha = val; }

private:
    QString _email, _password, _name, _captchaId, _captchaSolution;
    bool _createCaptcha;
};

class PasswordTokenRequest : public RequestInterface
{
public:
    PasswordTokenRequest();
    PasswordTokenRequest(QString email);

    QByteArray serialize() const;

    QString email() const { return _email; }
    void email(QString& val) { _email = val; }

private:
    QString _email;
};

} /* end SSO namespace */

#endif /* _REQUESTS_H_ */
