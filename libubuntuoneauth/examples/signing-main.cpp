/*
 * Copyright (C) 2013 Canonical Ltd.
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

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QTimer> 

#include "ssoservice.h"
#include "token.h"

#include "signing.h"

namespace UbuntuOne {

    SigningExample::SigningExample(QObject *parent) :
        QObject(parent)
    {
        QObject::connect(&service, SIGNAL(credentialsFound(const Token&)),
                         this, SLOT(handleCredentialsFound(Token)));
        QObject::connect(&service, SIGNAL(credentialsNotFound()),
                         this, SLOT(handleCredentialsNotFound()));
        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(handleNetworkFinished(QNetworkReply*)));

    }

    SigningExample::~SigningExample(){
    }

    void SigningExample::doExample()
    {
        service.getCredentials();
    }

    void SigningExample::handleCredentialsFound(Token token)
    {
        qDebug() << "Credentials found, signing url.";

        QString url = "https://one.ubuntu.com/api/account/";
        QString authHeader = token.signUrl(url, QStringLiteral("GET"));

        qDebug() << "URL Signed, authHeader is:" << authHeader;

        QNetworkRequest req;
        req.setRawHeader(QStringLiteral("Authorization").toUtf8(),
                          authHeader.toUtf8());
        req.setUrl(url);
        nam.get(req);
    }
 
    void SigningExample::handleCredentialsNotFound()
    {
        qDebug() << "No credentials were found.";
    }

    void SigningExample::handleNetworkFinished(QNetworkReply *reply)
    {
        QVariant statusAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if(!statusAttr.isValid()) {
            qDebug() << "Invalid HTTP response.";
            return;
        }

        int status = statusAttr.toInt();
        qDebug() << "HTTP Status " << status;

        if (status != 200){
            qDebug() << reply->rawHeaderPairs();
        }

        qDebug() << reply->readAll();
        
        emit finished();
    }

} // namespace UbuntuOne


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    
    UbuntuOne::SigningExample *example = new UbuntuOne::SigningExample(&a);

    QObject::connect(example, SIGNAL(finished()), &a, SLOT(quit()));

    QTimer::singleShot(0, example, SLOT(doExample()));

    return a.exec();
}


