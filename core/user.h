#ifndef USER_H
#define USER_H

#include <QObject>
#include "ssoservice.h"

namespace SSO {

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(SSOService *parent = 0);

signals:

public slots:
    bool sessionOpened();

private:
    SSOService* _service;

};

}

#endif // USER_H
