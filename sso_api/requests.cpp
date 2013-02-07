#include "requests.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace SSO {

RequestInterface::RequestInterface()
{
}

Token::Token()
    : RequestInterface(),
      _email(""), _password(""), _name(""), _otp("")
{
}

Token::Token(QString email, QString password, QString name, QString otp)
    : RequestInterface(),
      _email(email), _password(password), _name(name), _otp(otp)
{
}

QByteArray Token::Serialize() const
{
    QJsonObject serializer;
    serializer.insert("email", this->email());
    serializer.insert("password", this->password());
    serializer.insert("token_name", this->name());
    /* TODO: check for null */
    serializer.insert("otp", this->otp());

    QJsonDocument doc(serializer);

    /* TODO: Check what this returns in error case,
            may want to use the overload that takes a bool for success. */
    return doc.toJson();
}

Account::Account()
    : RequestInterface()
{
}

QByteArray Account::Serialize() const
{
    QJsonObject data;
    data.insert("email", this->email());
    data.insert("password", this->password());
    data.insert("displayname", this->name());
    data.insert("captcha_id", this->captchaId());
    data.insert("captcha_solution", this->captchaSolution());

    QJsonDocument doc(data);
    return doc.toJson();
}

} /* end SSO namespace */
