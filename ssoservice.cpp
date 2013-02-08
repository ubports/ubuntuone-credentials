/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Diego Sarmentero <diego.sarmentero@canonical.com>
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

#include "ssoservice.h"
#include <QDebug>

namespace SSO {

#define TOKEN_NAME "UbuntuSSO"

SSOService::SSOService(QObject *parent) :
    QObject(parent),
    _conn(QDBusConnection::sessionBus())
{
    // create the keyring that will be used to store and retrieve the different tokens
    _keyring = new keyring::Keyring(_conn);
    this->connect(_keyring, SIGNAL(sessionOpened()), this, SLOT(sessionDetected()));
    this->connect(_keyring, SIGNAL(credentialsFound(QString,QString,QString,bool)), this, SLOT(credentialsFound(QString,QString,QString)));
}

void SSOService::init_service()
{
    _keyring->openSession();
    // Open DBus listener
}

bool SSOService::sessionOpened()
{
    return this->_serviceEnabled;
}

void SSOService::sessionDetected()
{
    this->_serviceEnabled = true;
    emit this->sessionActivated();
//    this->_keyring->setCredentials("diego-sso", "asdtoken", "asdsecret");
//    this->_keyring->getCredentials("diego-sso");
}

void SSOService::credentialsFound(QString id, QString token, QString secret)
{
    qDebug() << "CREDENTIALS FOUND";
    qDebug() << id;
    qDebug() << token;
    qDebug() << secret;
}

void SSOService::registerUser(QString email, QString password, QString display_name)
{

}

void SSOService::login(QString email, QString password)
{

}

}
