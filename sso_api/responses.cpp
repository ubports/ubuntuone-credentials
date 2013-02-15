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

TokenResponse::TokenResponse()
    : _token_name(""), _token_secret(""), _token_key(""),
      _date_created(""), _date_updated(""),
      _consumer_secret(""), _consumer_key(""), _href("")
{
}

AccountResponse::AccountResponse()
    : _status(""), _openid(""), _displayname(""),
      _href(""), _email(""), _emails("")
{
}

} /* end SSO namespace */
