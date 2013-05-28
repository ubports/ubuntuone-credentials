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

#include "responses.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "errormessages.h"

namespace UbuntuOne {

ErrorResponse::ErrorResponse()
    : _httpStatus(-1), _httpReason(""), _message(""),
      _imageUrl(""), _captchaId(""), _captchaMessage(""), _email("")
{
    this->initializeMap();
    this->_code = -1;
}

ErrorResponse::ErrorResponse(const int& httpStatus, const QString& httpReason,
                             const QString& code, const QString& message)
    : _httpStatus(httpStatus), _httpReason(httpReason),
      _message(message), _imageUrl(""),
      _captchaId(""), _captchaMessage(""), _email("")
{
    this->initializeMap();
    this->code(code);
}

void ErrorResponse::initializeMap()
{
    this->_mapErrorEnum[LOGIN_FAILED] = CODE_LOGIN_FAILED;
    this->_mapErrorEnum[CAPTCHA_REQUIRED] = CODE_CAPTCHA_REQUIRED;
    this->_mapErrorEnum[INVALID_CREDENTIALS] = CODE_INVALID_CREDENTIALS;
    this->_mapErrorEnum[TWOFACTOR_REQUIRED] = CODE_TWOFACTOR_REQUIRED;
    this->_mapErrorEnum[ACCOUNT_SUSPENDED] = CODE_ACCOUNT_SUSPENDED;
    this->_mapErrorEnum[ACCOUNT_DEACTIVATED] = CODE_ACCOUNT_DEACTIVATED;
    this->_mapErrorEnum[EMAIL_INVALIDATED] = CODE_EMAIL_INVALIDATED;
    this->_mapErrorEnum[CAN_NOT_RESET_PASSWORD] = CODE_CAN_NOT_RESET_PASSWORD;
    this->_mapErrorEnum[ALREADY_REGISTERED] = CODE_ALREADY_REGISTERED;
}

void ErrorResponse::code(QString val)
{
    std::string code = val.toStdString();
    std::map<std::string, int>::iterator it;
    it = this->_mapErrorEnum.find(code);
    if(it != this->_mapErrorEnum.end()){
        this->_code = this->_mapErrorEnum[code];
    }else{
       this->_code = -1;
    }
}

OAuthTokenResponse::OAuthTokenResponse()
    : _token_name(""), _token_secret(""), _token_key(""),
      _date_created(""), _date_updated(""),
      _consumer_secret(""), _consumer_key(""), _href("")
{
}

PasswordTokenResponse::PasswordTokenResponse()
    : _email("")
{
}

AccountResponse::AccountResponse()
    : _status(""), _openid(""), _displayname(""),
      _href(""), _email(""), _emails("")
{
}

} /* end SSO namespace */
