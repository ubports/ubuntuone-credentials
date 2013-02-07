#include "identityprovider.h"

//#include <QVariantList>
//#include <QVariantMap>
#include <QJsonDocument>
#include <QDebug>

namespace SSO {

IdentityProvider::IdentityProvider(QObject *parent) :
    QObject(parent)
{
    /* TODO: either shared pointer or delete this */
    this->network = new Network(this);

    QObject::connect(this->network, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(reply_obtained(QNetworkReply*)));
}

void IdentityProvider::GetToken(const Token& token)
{
    this->network->Post(token.Serialize());
}

void IdentityProvider::CreateAccount(const Account& account)
{
    this->network->Post(account.Serialize());
}

void IdentityProvider::reply_obtained(QNetworkReply* reply)
{
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

    // Parse document
}

} /* end SSO namespace */
