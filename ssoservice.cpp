#include "ssoservice.h"
#include <QxtLogger>

namespace SSO {

#define TOKEN_NAME "UbuntuSSO"

SSOService::SSOService(QObject *parent) :
    QObject(parent),
    _conn(QDBusConnection::sessionBus())
{
    // create the keyring that will be used to store and retrieve the different tokens
    qxtLog->info() << "Initialize SSO Service...";
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
    qxtLog->debug() << "Session Detected";
    this->_serviceEnabled = true;
    emit this->sessionOpened();
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

void SSOService::createAccount(QString email, QString password, QString display_name)
{

}

void SSOService::getToken(QString email, QString password, QString token_name)
{

}

}
