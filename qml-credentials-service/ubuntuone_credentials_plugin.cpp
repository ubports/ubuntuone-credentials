#include "ubuntuone_credentials_plugin.h"
#include "ubuntuone_credentials_service.h"

#include <qqml.h>

void UbuntuOneCredentialsPlugin::registerTypes(const char *uri)
{
    // @uri Ubuntu.One.Components
    qmlRegisterType<UbuntuOneCredentialsService>(uri, 1, 0, "UbuntuOneCredentialsService");
}


