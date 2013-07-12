#ifndef UBUNTUONECREDENTIALS_PLUGIN_H
#define UBUNTUONECREDENTIALS_PLUGIN_H

#include <QQmlExtensionPlugin>

class UbuntuOneCredentialsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
    
public:
    void registerTypes(const char *uri);
};

#endif // UBUNTUONECREDENTIALS_PLUGIN_H

