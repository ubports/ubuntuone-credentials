#include "user.h"

namespace SSO {

User::User(SSOService *parent) :
    QObject(parent)
{
    this->_service = parent;
}

bool User::sessionOpened()
{
    return this->_service->sessionOpened();
}

}
