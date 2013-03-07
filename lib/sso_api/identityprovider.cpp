#include "identityprovider.h"
#include "responses.h"

#include <QDebug>

namespace SSO {

IdentityProvider::IdentityProvider(QObject *parent) :
    QObject(parent)
{
    /* TODO: either shared pointer or delete this */
    this->network = new Network(this);

    QObject::connect(this->network,
                     SIGNAL(OAuthTokenGranted(const OAuthTokenResponse&)),
                     this, SLOT(OnOAuthTokenGranted(const OAuthTokenResponse&)));
    QObject::connect(this->network,
                     SIGNAL(PasswordTokenGranted(const PasswordTokenResponse&)),
                     this, SLOT(OnPasswordTokenGranted(const PasswordTokenResponse&)));
    QObject::connect(this->network, SIGNAL(AccountGranted(const AccountResponse&)),
                     this, SLOT(OnAccountGranted(const AccountResponse&)));
    QObject::connect(this->network, SIGNAL(ErrorOccurred(const ErrorResponse&)),
                     this, SLOT(OnErrorOccurred(const ErrorResponse&)));
    }

void IdentityProvider::GetOAuthToken(const OAuthTokenRequest& token)
{
    this->network->Post(token);
}

void IdentityProvider::GetPasswordToken(const PasswordTokenRequest& token)
{
    this->network->Post(token);
}

void IdentityProvider::CreateAccount(const AccountRequest& account)
{
    this->network->Post(account);
}

/* Token API slots */

void IdentityProvider::OnOAuthTokenGranted(const OAuthTokenResponse& token)
{
    emit OAuthTokenGranted(token);

    qDebug() << "OAuth Token received!";
    qDebug() << "token_name: " << token.token_name();
    qDebug() << "token_secret: " << token.token_secret();
    qDebug() << "token_key: " << token.token_key();
    qDebug() << "date_created: " << token.date_created();
    qDebug() << "date_updated: " << token.date_updated();
    qDebug() << "consumer_secret: " << token.consumer_secret();
    qDebug() << "consumer_key: " << token.consumer_key();
    qDebug() << "href: " << token.href();
}

void IdentityProvider::OnPasswordTokenGranted(const PasswordTokenResponse& token)
{
    emit PasswordTokenGranted(token);

    qDebug() << "Password Token received!";
    qDebug() << "email: " << token.email();
}

void IdentityProvider::OnAccountGranted(const AccountResponse& account)
{
    emit AccountGranted(account);

    qDebug() << "Account granted!";
    qDebug() << "status" << account.status();
    qDebug() << "openid" << account.openid();
    qDebug() << "displayname" << account.displayname();
    qDebug() << "email" << account.email();
    qDebug() << "href" << account.href();
    qDebug() << "emails" << account.emails();
}

void IdentityProvider::OnErrorOccurred(const ErrorResponse& error)
{
    emit ErrorOccurred(error);

    qDebug() << "Error occurred!";
    qDebug() << "httpStatus" << error.httpStatus();
    qDebug() << "httpReason" << error.httpReason();
    qDebug() << "code" << error.code();
    qDebug() << "message" << error.message();

    qDebug() << "Extra info:";
    qDebug() << "imageUrl" << error.imageUrl();
    qDebug() << "captchaId" << error.captchaId();
    qDebug() << "captchaMessage" << error.captchaMessage();
    qDebug() << "email" << error.email();
}

} /* end SSO namespace */
