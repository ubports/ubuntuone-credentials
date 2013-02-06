#include "identityprovider.h"

//#include <QVariantList>
#include <QVariantMap>
#include <qjson/serializer.h>
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
    bool ok;
    QVariantMap result = parser.parse(reply->readAll(), &ok).toMap();
    if(!ok){
        return;
    }

    qDebug() << result["href"].toString();
//    qDebug() << result["key"].toString();
//    qDebug() << result["secret"].toString();
//    qDebug() << result["token_name"].toString();
}

} /* end SSO namespace */
