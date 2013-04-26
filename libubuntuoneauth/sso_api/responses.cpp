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

namespace SSO {

ErrorResponse::ErrorResponse()
    : _httpStatus(-1), _httpReason(""), _code(""),_message(""),
      _imageUrl(""), _captchaId(""), _captchaMessage(""), _email("")
{
}

ErrorResponse::ErrorResponse(const int& httpStatus, const QString& httpReason,
                             const QString& code, const QString& message)
    : _httpStatus(httpStatus), _httpReason(httpReason),
      _code(code), _message(message), _imageUrl(""),
      _captchaId(""), _captchaMessage(""), _email("")
{
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
