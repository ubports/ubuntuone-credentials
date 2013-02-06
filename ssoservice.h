#ifndef SSOSERVICE_H
#define SSOSERVICE_H

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include "keyring/keyring.h"

namespace SSO {

class SSOService : public QObject
{
    Q_OBJECT
public:
    explicit SSOService(QObject *parent = 0);

    void init_service();
    bool sessionOpened();
    void createAccount(QString email, QString password, QString display_name);
    void getToken(QString email, QString password, QString token_name);

signals:
    void sessionOpened();
    // Account Fail
    void alreadyRegistered();
    void captchaFailure();
    void captchaRequired();
    // Token Fail
    void invalidCredentials();
    void accountSuspended();
    void accountDeactivated();
    void twoFactorRequired();
    void twoFactorFailure();

private slots:
    void sessionDetected();
    void credentialsFound(QString id, QString token, QString secret);

private:
    bool _serviceEnabled = false;
    QDBusConnection _conn;
    keyring::Keyring* _keyring;
};

}

#endif // SSOSERVICE_H
