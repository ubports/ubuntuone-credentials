#ifndef _IDENTITYPROVIDER_H_
#define _IDENTITYPROVIDER_H_

#include "network.h"
#include "requests.h"
#include "responses.h"

#include <QObject>

namespace SSO { /* TODO: is this a good/correct name? */

class IdentityProvider : public QObject
{
    Q_OBJECT
public:
    explicit IdentityProvider(QObject *parent = 0);

    void GetOAuthToken(const OAuthTokenRequest& token);
    void GetPasswordToken(const PasswordTokenRequest& token);
    void CreateAccount(const AccountRequest& account);

signals:
    void OAuthTokenGranted(const OAuthTokenResponse& token);
    void PasswordTokenGranted(const PasswordTokenResponse& token);
    void AccountGranted(const AccountResponse& account);
    void ErrorOccurred(const ErrorResponse& error);

public slots:
    void OnOAuthTokenGranted(const OAuthTokenResponse& token);
    void OnPasswordTokenGranted(const PasswordTokenResponse& token);
    void OnAccountGranted(const AccountResponse& account);
    void OnErrorOccurred(const ErrorResponse& error);

private slots:

private:
    Network* network;
};

} /* end SSO namespace */

#endif // _IDENTITYPROVIDER_H_
