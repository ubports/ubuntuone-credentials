#ifndef _REQUESTS_H_
#define _REQUESTS_H_

#include <QString>
#include <QUrl>

namespace SSO {

static QString API_BASE = "https://login.ubuntu.com/api/v2/";

class RequestInterface
{
public:
    RequestInterface(const QUrl& url);

    virtual QByteArray serialize() const = 0;
    QUrl url() const { return _url; }

protected:
    QUrl _url;
};

class TokenRequest : public RequestInterface
{
public:
    TokenRequest();
    TokenRequest(QString email, QString password, QString token, QString otp);

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

} /* end SSO namespace */

#endif /* _REQUESTS_H_ */
