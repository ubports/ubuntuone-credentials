#ifndef SIGNAL_MAPPER_H
#define SIGNAL_MAPPER_H

#include <QDBusPendingCallWatcher>
#include <QList>
#include <QObject>

namespace dbus
{

// this is inspired by the QSignalMapper class
class DBusSignalMapperPrivate;
class DBusSignalMapper : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DBusSignalMapper)
public:
    explicit DBusSignalMapper(QObject* parent = 0);
    ~DBusSignalMapper();

    void setMapping(QObject* sender, int id);
    void setMapping(QObject* sender, const QString& text);
    void setMapping(QObject* sender, QWidget* widget);
    void setMapping(QObject* sender, QObject* object);
    void removeMappings(QObject* sender);

    QObject* mapping(int id) const;
    QObject* mapping(const QString& text) const;
    QObject* mapping(QWidget* widget) const;
    QObject* mapping(QObject* object) const;

    static class _init
    {
        public:
            _init()
            {
                qRegisterMetaType<QDBusPendingCallWatcher*>("QDBusPendingCallWatcher*");
            }
    } _initializer;

Q_SIGNALS:
    void mapped(QDBusPendingCallWatcher* watcher, int id);
    void mapped(QDBusPendingCallWatcher* watcher, const QString& text);
    void mapped(QDBusPendingCallWatcher* watcher, QWidget* widget);
    void mapped(QDBusPendingCallWatcher* watcher, QObject* object);

public Q_SLOTS:
    void map(QDBusPendingCallWatcher* watcher);
    void map(QDBusPendingCallWatcher* watcher, QObject* sender);

protected:
    QScopedPointer<DBusSignalMapperPrivate> d_ptr;
    QList<QObject*> intMaps();
    QList<QObject*> stringMaps();
    QList<QObject*> widgetMaps();
    QList<QObject*> objectMaps();

private:
    Q_DISABLE_COPY(DBusSignalMapper)
    Q_PRIVATE_SLOT(d_func(), void _q_senderDestroyed())
};

} // dbus

#endif // SIGNAL_MAPPER_H
