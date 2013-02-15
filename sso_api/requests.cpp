#include "requests.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace SSO {

RequestInterface::RequestInterface(const QUrl& url)
    : _url(url)
{
}

TokenRequest::TokenRequest()
    : RequestInterface(API_BASE + "tokens"),
      _email(""), _password(""), _name(""), _otp("")
{
}

TokenRequest::TokenRequest(QString email, QString password,
                           QString name, QString otp)
    : RequestInterface(API_BASE + "tokens"),
      _email(email), _password(password), _name(name), _otp(otp)
{
}

QByteArray TokenRequest::serialize() const
{
    QJsonObject serializer;
    serializer.insert("email", this->email());
    serializer.insert("password", this->password());
    serializer.insert("token_name", this->name());
    /* TODO: check for null */
    if (!this->otp().isEmpty())
        serializer.insert("otp", this->otp());

    QJsonDocument doc(serializer);

    return doc.toJson();
}

AccountRequest::AccountRequest()
    : RequestInterface(API_BASE + "accounts"),
      _email(""), _password(""), _name(""),
      _captchaId(""), _captchaSolution(""),
      _createCaptcha(true)
{
}

AccountRequest::AccountRequest(QString email, QString password, QString name,
                               QString captchaId, QString captchaSolution,
                               bool createCaptcha)
    : RequestInterface(API_BASE + "accounts"),
      _email(email), _password(password), _name(name),
      _captchaId(captchaId), _captchaSolution(captchaSolution),
      _createCaptcha(true)
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

} /* end SSO namespace */
