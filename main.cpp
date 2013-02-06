#include <QCoreApplication>
#include "keyring/keyring_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    KeyringManager* manager = new KeyringManager();
    manager->start();
    
    return a.exec();
}
