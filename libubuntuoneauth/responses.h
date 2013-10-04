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

#ifndef _RESPONSES_H_
#define _RESPONSES_H_

#include <map>
#include <string>
#include <QString>
#include <QStringList>

namespace UbuntuOne {

class ErrorResponse
{
public:
    ErrorResponse();
    ErrorResponse(const int& httpStatus, const QString& httpReason,
                  const QString& code, const QString& message);

    int httpStatus() const { return _httpStatus; }
    QString httpReason() const { return _httpReason; }
    int code() const { return _code; }
    QString message() const { return _message; }
    QString imageUrl() const { return _imageUrl; }
    QString captchaId() const { return _captchaId; }
    QString captchaMessage() const { return _captchaMessage; }
    QString email() const { return _email; }

    void httpStatus(int val) { _httpStatus = val; }
    void httpReason(QString val) { _httpReason = val; }
    void code(QString val);
    void message(QString val) { _message = val; }
    void imageUrl(QString val) { _imageUrl = val; }
    void captchaId(QString val) { _captchaId = val; }
    void captchaMessage(QString val) { _captchaMessage = val; }
    void email(QString val) { _email = val; }

private:
    int _httpStatus;
    QString _httpReason, _message, _imageUrl,
            _captchaId, _captchaMessage, _email;
    int _code;

    // Map to associate the strings with the enum values
    std::map<std::string, int> _mapErrorEnum;
    void initializeMap();
};

class OAuthTokenResponse
{
public:
    OAuthTokenResponse();

    QString token_name() const { return _token_name; }
    QString token_secret() const { return _token_secret; }
    QString token_key() const { return _token_key; }
    QString date_created() const { return _date_created; }
    QString date_updated() const { return _date_updated; }
    QString consumer_secret() const { return _consumer_secret; }
    QString consumer_key() const { return _consumer_key; }
    QString href() const { return _href; }

    void token_name(QString val) { _token_name = val; }
    void token_secret(QString val) { _token_secret = val; }
    void token_key(QString val) { _token_key = val; }
    void date_created(QString val) { _date_created = val; }
    void date_updated(QString val) { _date_updated = val; }
    void consumer_secret(QString val) { _consumer_secret = val; }
    void consumer_key(QString val) { _consumer_key = val; }
    void href(QString val) { _href = val; }

private:
    QString _token_name, _token_secret, _token_key,
            _date_created, _date_updated,
            _consumer_secret, _consumer_key, _href;
};

class PasswordTokenResponse
{
public:
    PasswordTokenResponse();

    QString email() const { return _email; }

    void email(QString val) { _email = val; }

private:
    QString _email;
  
};

class AccountResponse
{
public:
    AccountResponse();

    QString status() const { return _status; }
    QString openid() const { return _openid; }
    QString displayname() const { return _displayname; }
    QString href() const { return _href; }
    QString email() const { return _email; }
    QStringList emails() const { return _emails; }

    void status(QString val) { _status = val; }
    void openid(QString val) { _openid = val; }
    void displayname(QString val) { _displayname = val; }
    void href(QString val) { _href = val; }
    void email(QString val) { _email = val; }
    void emails(QStringList val) { _emails = val; }

private:
    QString _status, _openid, _displayname, _href, _email;
    QStringList _emails;
};

} /* end UbuntuOne namespace */

#endif /* _RESPONSES_H_ */
