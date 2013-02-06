#include "requests.h"

//#include <QVariantList>
#include <QVariantMap>
#include <qjson/serializer.h>
#include <QDebug>

namespace SSO {

RequestInterface::RequestInterface()
{
    /* TODO: delete */
    this->serializer = new QJson::Serializer();
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
    QVariantMap data;
    data.insert("email", this->email());
    data.insert("password", this->password());
    data.insert("token_name", this->name());
    /* TODO: check for null */
    data.insert("otp", this->otp());

    /* TODO: Check what this returns in error case,
            may want to use the overload that takes a bool for success. */
    return this->serializer->serialize(data);
}

Account::Account()
    : RequestInterface()
{
}

QByteArray Account::Serialize() const
{
    QVariantMap data;
    data.insert("email", this->email());
    data.insert("password", this->password());
    data.insert("displayname", this->name());
    data.insert("captcha_id", this->captchaId());
    data.insert("captcha_solution", this->captchaSolution());
    
    return this->serializer->serialize(data);
}

} /* end SSO namespace */
