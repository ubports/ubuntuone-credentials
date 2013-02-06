#ifndef INTERFACE_FACTORY_H
#define INTERFACE_FACTORY_H

#include "collection.h"
#include "item.h"
#include "service.h"

class InterfaceFactory
{

public:
    InterfaceFactory();
    ~InterfaceFactory();

    template <class T>
    T* create(const QString &service, const QString &path,
        const QDBusConnection &connection, QObject *parent = 0)
    {
        return new T(service, path, connection, parent);
    }

};

#endif // INTERFACE_FACTORY_H
