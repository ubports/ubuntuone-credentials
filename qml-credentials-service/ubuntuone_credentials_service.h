#ifndef UBUNTUONE_CREDENTIALS_SERVICE_H
#define UBUNTUONE_CREDENTIALS_SERVICE_H

#include <QQuickItem>
#include "ssoservice.h"

using namespace UbuntuOne;

enum CredentialsServiceState{
    IDLE = 0,
    CHECK,
    LOGIN,
    REGISTER,
    SIGN
};

class UbuntuOneCredentialsService : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(UbuntuOneCredentialsService)

public:
    UbuntuOneCredentialsService(QQuickItem *parent = 0);
    ~UbuntuOneCredentialsService();

    Q_INVOKABLE void checkCredentials();
    Q_INVOKABLE void login(QString email, QString password, QString twoFactorCode = QString());
    Q_INVOKABLE void registerUser(QString email, QString password, QString name);
    Q_INVOKABLE void signUrl(QString url, QString method, bool asQuery = false);

signals:
    void credentialsFound();
    void credentialsNotFound();
    void loginOrRegisterSuccess();
    void loginOrRegisterError(QString errorMessage);
    void twoFactorAuthRequired();
    void urlSigned(QString signedUrl);
    void urlSigningError(QString errorMessage);

private slots:
    void handleCredentialsFound(const Token&);
    void handleCredentialsNotFound();
    void handleCredentialsStored();
    void handleTwoFactorAuthRequired();
    void handleError(const ErrorResponse&);

private:
    SSOService _service;
    CredentialsServiceState _state;
    QString _sign_url;
    QString _sign_method;
    bool _sign_asQuery;
};

QML_DECLARE_TYPE(UbuntuOneCredentialsService)

#endif // UBUNTUONE_CREDENTIALS_SERVICE_H

