#include "keyring_manager.h"
#include <QDebug>

KeyringManager::KeyringManager(QObject *parent) :
    QObject(parent),
  _conn(QDBusConnection::sessionBus())
{
    // create the keyring that will be used to store and retrieve the different
    // tokens
    _keyring = new keyring::Keyring(_conn);
    this->connect(_keyring, SIGNAL(sessionOpened()), this, SLOT(sessionDetected()));
    this->connect(_keyring, SIGNAL(credentialsFound(QString,QString,QString,bool)), this, SLOT(credentialsFound(QString,QString,QString)));
}

void KeyringManager::start()
{
    _keyring->openSession();
}

void KeyringManager::sessionDetected()
{
//    this->_keyring->setCredentials("diego-sso", "asdtoken", "asdsecret");
    this->_keyring->getCredentials("diego-sso");
}

void KeyringManager::credentialsFound(QString id, QString token, QString secret)
{
    qDebug() << "CREDENTIALS FOUND";
    qDebug() << id;
    qDebug() << token;
    qDebug() << secret;
}
