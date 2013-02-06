#include "network.h"

//#include <QVariantList>
#include <QVariantMap>
#include <QDebug>

namespace SSO {

const QString Network::_tokensApi = API_BASE + "tokens";
const QString Network::_accountsApi = API_BASE + "accounts";

Network::Network(QObject *parent)
    : QObject(parent)
{
    /* either shared pointer or delete this */
    this->_nam = new QNetworkAccessManager(this);
    this->_request = new QNetworkRequest();

    /*this->_tokensApi = API_BASE + "tokens";
    this->_accountsApi = API_BASE + "accounts";*/

    this->_request->setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/json");

    QObject::connect(this->_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(OnReply(QNetworkReply*)));
}

void Network::Post(const QByteArray& bytes)
{
    /* map request types to their QUrls? */
    this->_request->setUrl(QUrl(this->_tokensApi));
    this->_nam->post(*this->_request, bytes);
}

void Network::OnReply(QNetworkReply* reply)
{
    qDebug() << "OnReply!";
    /*
    bool ok;
    QVariantMap result = parser.parse(reply->readAll(), &ok).toMap();
    if(!ok){
        return;
    }

    qDebug() << result["href"].toString();
    qDebug() << result["key"].toString();
    qDebug() << result["secret"].toString();
    qDebug() << result["token_name"].toString();
    */
}

} /* end SSO namespace */
