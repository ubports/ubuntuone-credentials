/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "common.h"
#include "responses.h"

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace UbuntuOne {

class U1_EXPORT Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    template <class T>
    void Post(const T& request)
    {
        _request->setUrl(request.url());
        _nam->post(*_request, request.serialize());
    }

signals:
    void OAuthTokenGranted(const OAuthTokenResponse& token);
    void PasswordTokenGranted(const PasswordTokenResponse& token);
    void AccountGranted(const AccountResponse& account);
    void ErrorOccurred(const ErrorResponse& error);
    
public slots:

private slots:
    void OnReply(QNetworkReply*);

private:
    void ProcessOAuthTokenReply(const QJsonObject& object);
    void ProcessPasswordTokenReply(const QJsonObject& object);
    void ProcessAccountsReply(const QJsonObject& object);

    QNetworkAccessManager* _nam;
    QNetworkRequest* _request;

    static const QString _tokensApi, _accountsApi;
};

} /* end UbuntuOne namespace */

#endif /* _NETWORK_H_ */
