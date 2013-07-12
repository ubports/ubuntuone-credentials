#include "ubuntuone_credentials_plugin.h"
#include "ubuntuone_credentials_service.h"

#include <qqml.h>

void UbuntuOneCredentialsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<UbuntuOneCredentialsService>(uri, 1, 0, "UbuntuOneCredentialsService");
}


