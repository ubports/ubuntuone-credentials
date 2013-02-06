#ifndef KEYRING_MANAGER_H
#define KEYRING_MANAGER_H

#include <QObject>
#include <QDBusConnection>
#include "keyring.h"

class KeyringManager : public QObject
{
    Q_OBJECT
public:
    explicit KeyringManager(QObject *parent = 0);

    void start();
    
signals:
    
public slots:
    void sessionDetected();
    void credentialsFound(QString id, QString token, QString secret);

private:
    QDBusConnection _conn;
    keyring::Keyring* _keyring;
};

#endif // KEYRING_MANAGER_H
