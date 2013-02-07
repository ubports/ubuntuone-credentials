#ifndef _IDENTITYPROVIDER_H_
#define _IDENTITYPROVIDER_H_

#include "network.h"
#include "requests.h"

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace SSO { /* TODO: is this a good/correct name? */

class IdentityProvider : public QObject
{
    Q_OBJECT
public:
    explicit IdentityProvider(QObject *parent = 0);

    void GetToken(const Token& token);
    /*void AddOtp();*/
    /*void AddOtpHandler();*/

    void CreateAccount(const Account& account);
    /*void AddCaptcha();*/
    /*void AddCaptchaHandler();*/

signals:

public slots:

private slots:
    void reply_obtained(QNetworkReply*);

private:
    Network* network;
};

} /* end SSO namespace */

#endif // _IDENTITYPROVIDER_H_
