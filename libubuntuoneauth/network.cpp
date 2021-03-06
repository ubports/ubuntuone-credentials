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
#include <QVariantMap>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QStringList>

#include "errormessages.h"
#include "network.h"
#include "responses.h"
#include "requests.h"

namespace UbuntuOne {

Network::Network(QObject *parent)
    : QObject(parent)
{
    /* either shared pointer or delete this */
    _nam = new QNetworkAccessManager(this);
    _request = new QNetworkRequest();

    _request->setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/json");

    QObject::connect(_nam, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(OnReply(QNetworkReply*)));
}

void Network::OnReply(QNetworkReply* reply)
{
    QVariant statusAttr = reply->attribute(
                            QNetworkRequest::HttpStatusCodeAttribute);
    /* TODO: see if we really need to do this check, we could just operate
       on a bad value as being an error rather than the extra check? */
    if (!statusAttr.isValid()) {
        QString errmsg = QString("Invalid status attribute in Network::OnReply");
        // Use login failed code, which results in generic error message:
        emit ErrorOccurred(ErrorResponse(0, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
        return;
    }

    int httpStatus = statusAttr.toInt();

    qDebug() << "Network::OnReply from " << reply->url()
             << " status: " << httpStatus;
    
    QByteArray payload = reply->readAll();
    if (payload.isEmpty()) {
        QString errmsg = QString("Network::OnReply: empty payload, giving up.");
        emit ErrorOccurred(ErrorResponse(0, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
        return;
    }
 
    QJsonParseError jsonParseError;
    QJsonDocument document = QJsonDocument::fromJson(payload, &jsonParseError);

    if(jsonParseError.error != QJsonParseError::NoError){
        QString errmsg = QString ("Network::OnReply: error parsing JSON reply: ");
        errmsg += jsonParseError.errorString();
        emit ErrorOccurred(ErrorResponse(0, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
        return;
    }

    if (document.isEmpty() || document.isNull()) {
        QString errmsg = QString("Network::OnReply: received empty or Null document");
        emit ErrorOccurred(ErrorResponse(0, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
        return;
    }

    if (!document.isObject()) {
        QString errmsg = QString("Network::OnReply: received JSON document that isn't an object.");
        emit ErrorOccurred(ErrorResponse(0, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
        return;
    }
    
    QJsonObject object = document.object();

    /* TODO: map out urls to reply parsers */
    if (httpStatus == 200 || httpStatus == 201) {
        if (reply->url().path() == OAUTH_PATH)
            ProcessOAuthTokenReply(object);
        else if (reply->url().path() == PASSWORD_PATH)
            ProcessPasswordTokenReply(object);
        else if (reply->url().path() == ACCOUNTS_PATH)
            ProcessAccountsReply(object);
    } else /* Statuses outside 200, 201 are fails */ {
        QVariant phraseAttr = reply->attribute(
                            QNetworkRequest::HttpReasonPhraseAttribute);
        if (!phraseAttr.isValid()) {
            QString errmsg = QString("HTTP reason phrase is invalid");
            emit ErrorOccurred(ErrorResponse(httpStatus, NO_HTTP_REASON, LOGIN_FAILED, errmsg));
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

void Network::ProcessPasswordTokenReply(const QJsonObject& object)
{
    PasswordTokenResponse response;
    response.email(object.value("email").toString());

    emit PasswordTokenGranted(response);
}

void Network::ProcessOAuthTokenReply(const QJsonObject& object)
{
    /* TODO: may be able to speed this up by working with a map
             internal to the QJsonDocument, or something like that.*/

    OAuthTokenResponse response;
    response.token_name(object.value("token_name").toString());
    response.token_secret(object.value("token_secret").toString());
    response.token_key(object.value("token_key").toString());
    response.date_created(object.value("date_created").toString());
    response.date_updated(object.value("date_updated").toString());
    response.consumer_secret(object.value("consumer_secret").toString());
    response.consumer_key(object.value("consumer_key").toString());
    response.href(object.value("href").toString());

    emit OAuthTokenGranted(response);
}

} /* end UbuntuOne namespace */
