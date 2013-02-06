#ifndef _REQUESTS_H_
#define _REQUESTS_H_

#include <QString>
#include <qjson/serializer.h>

namespace SSO {

class RequestInterface
{
public:
    RequestInterface();
    virtual QByteArray Serialize() const = 0;
protected:
    QJson::Serializer* serializer;
};

class Token : public RequestInterface
{
public:
    Token();
    Token(QString email, QString password, QString token, QString otp);

    QByteArray Serialize() const;

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

class Account : public RequestInterface
{
public:
    Account();
    Account(QString email, QString password, QString name,
            QString captchaId, QString captchSolution);

    QByteArray Serialize() const;

    QString email() const { return _email; }
    QString password() const { return _password; }
    QString name() const { return _name; }
    QString captchaId() const { return _captchaId; }
    QString captchaSolution() const { return _captchaSolution; }
 
    void email(QString& val) { _email = val; }
    void password(QString& val) { _password = val; }
    void name(QString& val) { _name = val; }
    void captchaId(QString& val) { _captchaId = val; }
    void captchaSolution(QString& val) { _captchaSolution = val; }

private:
    QString _email, _password, _name, _captchaId, _captchaSolution;
};

} /* end SSO namespace */

#endif /* _REQUESTS_H_ */
