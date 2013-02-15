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
#include "responses.h"
#include "requests.h"

//#include <QVariantList>
#include <QVariantMap>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QStringList>

namespace SSO {

Network::Network(QObject *parent)
    : QObject(parent)
{
    /* either shared pointer or delete this */
    this->_nam = new QNetworkAccessManager(this);
    this->_request = new QNetworkRequest();

    this->_request->setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/json");

    QObject::connect(this->_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(OnReply(QNetworkReply*)));
}

void Network::OnReply(QNetworkReply* reply)
{
    QVariant statusAttr = reply->attribute(
                            QNetworkRequest::HttpStatusCodeAttribute);
    /* TODO: see if we really need to do this check, we could just operate
       on a bad value as being an error rather than the extra check? */
    if (!statusAttr.isValid()) {
        qDebug() << "invalid status received!";
        return;
    }

    int httpStatus = statusAttr.toInt();

    qDebug() << "Network::OnReply from " << reply->url();
    qDebug() << "Network::OnReply status: " << httpStatus;
    
    QByteArray payload = reply->readAll();
    if (payload.isEmpty()) {
        qDebug() << "empty payload";
        return; /* TODO: Do something to signal we're having a bad time. */
    }
 
    QJsonDocument document = QJsonDocument::fromJson(payload);
    /* TODO: add logging or raise some type of error signal? */
    if (document.isEmpty()) {
        qDebug() << "oops, received empty document";
        return;
    }

    if (!document.isObject()) {
        qDebug() << "uh oh, this isn't good.";
        return;
    }
    
    QJsonObject object = document.object();

    /* TODO: map out urls to reply parsers */
    if (httpStatus == 200 || httpStatus == 201) {
        if (reply->url() == API_BASE + "tokens")
            ProcessTokensReply(object);
        else if (reply->url() == API_BASE + "accounts")
            ProcessAccountsReply(object);
    } else /* Statuses outside 200, 201 are fails */ {
        QVariant phraseAttr = reply->attribute(
                            QNetworkRequest::HttpReasonPhraseAttribute);
        if (!phraseAttr.isValid()) {
            qDebug() << "got a bad HTTP reason";
            return;
        }
        QString httpReason = phraseAttr.toString();

        QString code = object.value("code").toString();
        QString message = object.value("message").toString();

        QJsonValue extra = object.value("extra");

        /* TODO: fill in details for each type of status */
        ErrorResponse error(httpStatus, httpReason, code, message);

        /* Some statuses provide extra information! */
        switch (httpStatus) {
        case 401:
            if (extra.isObject()) {
                QJsonObject ob = extra.toObject();
                QString url = ob.value("image_url").toString();
                QString id = ob.value("captcha_id").toString();

                error.imageUrl(url);
                error.captchaId(id);
            }
            break;
        case 403:
            if (extra.isObject()) {
                QJsonObject ob = extra.toObject();
                QString msg = ob.value("captcha_message").toString();

                error.captchaMessage(msg);
            }
            break;
        case 409:
            if (extra.isObject()) {
                QJsonObject ob = extra.toObject();
                QString email = ob.value("email").toString();

                error.email(email);
            }
            break;
        }
    
        emit ErrorOccurred(error);
        return;
    }
   
}

void Network::ProcessAccountsReply(const QJsonObject& object)
{
    AccountResponse response;
    response.status(object.value("status").toString());
    response.openid(object.value("openid").toString());
    response.displayname(object.value("displayname").toString());
    response.email(object.value("email").toString());
    response.href(object.value("href").toString());

    QJsonValue emails = object.value("emails");
    QStringList list;
    if (emails.isArray()) {
        QJsonArray array = emails.toArray();
        foreach(const QJsonValue& val, array) {
            if (val.isObject()) {
                /* TODO: this is dirty... */
                list.append(val.toObject().value("href").toString());
            }
        }
    }

    response.emails(list);

    emit AccountGranted(response);
}

void Network::ProcessTokensReply(const QJsonObject& object)
{
    /* TODO: may be able to speed this up by working with a map
             internal to the QJsonDocument, or something like that.*/

    TokenResponse response;
    response.token_name(object.value("token_name").toString());
    response.token_secret(object.value("token_secret").toString());
    response.token_key(object.value("token_key").toString());
    response.date_created(object.value("date_created").toString());
    response.date_updated(object.value("date_updated").toString());
    response.consumer_secret(object.value("consumer_secret").toString());
    response.consumer_key(object.value("consumer_key").toString());
    response.href(object.value("href").toString());

    emit TokenGranted(response);
}

} /* end SSO namespace */
