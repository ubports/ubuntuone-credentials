/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Diego Sarmentero <diego.sarmentero@canonical.com>
 * Author: Brian Curtin <brian.curtin@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

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
