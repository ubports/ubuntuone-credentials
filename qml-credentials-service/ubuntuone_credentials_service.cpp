#include "ubuntuone_credentials_service.h"
#include <QDebug>

UbuntuOneCredentialsService::UbuntuOneCredentialsService(QQuickItem *parent):
    QQuickItem(parent)
{
    QObject::connect(&(this->_service), SIGNAL(credentialsFound(Token)),
                     this, SLOT(handleCredentialsFound(Token)));
    QObject::connect(&(this->_service), SIGNAL(credentialsNotFound()),
                     this, SLOT(handleCredentialsNotFound()));
    QObject::connect(&(this->_service), SIGNAL(credentialsStored()),
                     this, SLOT(handleCredentialsStored()));

    QObject::connect(&(this->_service), SIGNAL(requestFailed(ErrorResponse)),
                     this, SLOT(handleError(ErrorResponse)));
}

UbuntuOneCredentialsService::~UbuntuOneCredentialsService()
{
}


// public API (Q_INVOKABLE)

void UbuntuOneCredentialsService::checkCredentials()
{
    Q_ASSERT(_state == IDLE);
    _state = CHECK;
    _service.getCredentials();
}

void UbuntuOneCredentialsService::login(QString email, QString password)
{
    Q_ASSERT(_state == IDLE);
    _state = LOGIN;
    _service.login(email, password);
}

void UbuntuOneCredentialsService::registerUser(QString email, QString password, QString name)
{
    Q_ASSERT(_state == IDLE);
    _state = REGISTER;
    _service.registerUser(email, password, name);
}

// calling signUrl with no credentials stored will emit an error
void UbuntuOneCredentialsService::signUrl(const QString url, const QString method, bool asQuery)
{
    Q_ASSERT(_state == IDLE);
    _state = SIGN;
    _sign_url = url;
    _sign_method = method;
    _sign_asQuery = asQuery;
    _service.getCredentials();
}

// signal handlers

// credentials found is only sent by _service.getCredentials(), not login or register
void UbuntuOneCredentialsService::handleCredentialsFound(const Token& token)
{
    QString signedUrl;

    qDebug() << "in UbuntuOneCredentialsService::handleCredentialsFound";

    // set state before emitting any signals, as signals happen 'after' transition
    CredentialsServiceState calledState = _state;
    _state = IDLE;

    switch(calledState){
    case CHECK:
        emit credentialsFound();   
        break;
    case SIGN:
        signedUrl = token.signUrl(_sign_url, _sign_method, _sign_asQuery);
        emit urlSigned(signedUrl);
        break;
    default:
        qDebug() << "UbuntuOneCredentialsService did not expect credentialsFound in state " << calledState;
    }
  
}

void UbuntuOneCredentialsService::handleCredentialsNotFound()
{
    qDebug() << "in UbuntuOneCredentialsService::handleCredentialsNotFound";
    
    // set state before emitting any signals, as signals happen 'after' transition
    CredentialsServiceState calledState = _state;
    _state = IDLE;

    switch(calledState){
    case CHECK:
        emit credentialsNotFound();
        break;
    case SIGN:
        emit urlSigningError(QString("Attempt to sign without credentials in keyring"));
        break;
    default:
        qDebug() << "UbuntuOneCredentialsService did not expect credentialsNotFound in state " << calledState;
    }
}

// Credentials stored is signaled when the u1 ping is done after login/register
void UbuntuOneCredentialsService::handleCredentialsStored()
{
    qDebug() << "in UbuntuOneCredentialsService::handleCredentialsStored";

    // set state before emitting any signals, as signals happen 'after' transition
    CredentialsServiceState calledState = _state;
    _state = IDLE;

    if (calledState == LOGIN || calledState == REGISTER){
        emit loginOrRegisterSuccess();
    }else{
        qDebug() << "UbuntuOneCredentialsService did not expect credentialsStored in state " << calledState;
    }
}

void UbuntuOneCredentialsService::handleError(const ErrorResponse& error)
{
    qDebug() << "in handleError\n";
    _state = IDLE;
    emit loginOrRegisterError(error.message());
}
