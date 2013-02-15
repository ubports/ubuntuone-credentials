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

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "responses.h"

#include <QDebug>
#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace SSO {

//static QString API_BASE = "https://login.ubuntu.com/api/v2/";

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    template <class T>
    void Post(const T& request)
    {
        qDebug() << request.url();
        this->_request->setUrl(request.url());
        this->_nam->post(*this->_request, request.serialize());
    }

signals:
    void TokenGranted(const TokenResponse& token);
    void AccountGranted(const AccountResponse& account);
    void ErrorOccurred(const ErrorResponse& error);
    
public slots:

private slots:
    void OnReply(QNetworkReply*);

private:
    void ProcessTokensReply(const QJsonObject& object);
    void ProcessAccountsReply(const QJsonObject& object);

    QNetworkAccessManager* _nam;
    QNetworkRequest* _request;

    static const QString _tokensApi, _accountsApi;
};

} /* end SSO namespace */

#endif /* _NETWORK_H_ */
