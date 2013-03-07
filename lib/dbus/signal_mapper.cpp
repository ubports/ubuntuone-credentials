/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Manuel de la Pena <manuel.delapena@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <QHash>
#include "signal_mapper.h"

namespace dbus
{

DBusSignalMapper::_init DBusSignalMapper::_initializer;

class DBusSignalMapperPrivate
{
    Q_DECLARE_PUBLIC(DBusSignalMapper)
 public:
    DBusSignalMapperPrivate(DBusSignalMapper* parent) :
        q_ptr(parent) {}

    void _q_senderDestroyed() {
        Q_Q(DBusSignalMapper);
        q->removeMappings(q->sender());
    }
    QHash<QObject*, int> intHash;
    QHash<QObject*, QString> stringHash;
    QHash<QObject*, QWidget*> widgetHash;
    QHash<QObject*, QObject*> objectHash;

private:
    DBusSignalMapper* q_ptr;
};

DBusSignalMapper::DBusSignalMapper(QObject *parent) :
    QObject(parent),
    d_ptr(new DBusSignalMapperPrivate(this))
{
}

DBusSignalMapper::~DBusSignalMapper()
{
}

void DBusSignalMapper::setMapping(QObject* sender, int id)
{
    Q_D(DBusSignalMapper);
    d->intHash.insert(sender, id);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void DBusSignalMapper::setMapping(QObject* sender, const QString &text)
{
    Q_D(DBusSignalMapper);
    d->stringHash.insert(sender, text);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void DBusSignalMapper::setMapping(QObject* sender, QWidget *widget)
{
    Q_D(DBusSignalMapper);
    d->widgetHash.insert(sender, widget);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void DBusSignalMapper::setMapping(QObject* sender, QObject *object)
{
    Q_D(DBusSignalMapper);
    d->objectHash.insert(sender, object);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

QObject* DBusSignalMapper::mapping(int id) const
{
    Q_D(const DBusSignalMapper);
    return d->intHash.key(id);
}

QObject* DBusSignalMapper::mapping(const QString& id) const
{
    Q_D(const DBusSignalMapper);
    return d->stringHash.key(id);
}

QObject* DBusSignalMapper::mapping(QWidget* widget) const
{
    Q_D(const DBusSignalMapper);
    return d->widgetHash.key(widget);
}

QObject* DBusSignalMapper::mapping(QObject* object) const
{
    Q_D(const DBusSignalMapper);
    return d->objectHash.key(object);
}

void DBusSignalMapper::removeMappings(QObject* sender)
{
    Q_D(DBusSignalMapper);

    d->intHash.remove(sender);
    d->stringHash.remove(sender);
    d->widgetHash.remove(sender);
    d->objectHash.remove(sender);
}

void DBusSignalMapper::map(QDBusPendingCallWatcher* watcher)
{
    map(watcher, sender());
}

void DBusSignalMapper::map(QDBusPendingCallWatcher* watcher, QObject* sender)
{
    Q_D(DBusSignalMapper);
    if (d->intHash.contains(sender))
        emit mapped(watcher, d->intHash.value(sender));
    if (d->stringHash.contains(sender))
        emit mapped(watcher, d->stringHash.value(sender));
    if (d->widgetHash.contains(sender))
        emit mapped(watcher, d->widgetHash.value(sender));
    if (d->objectHash.contains(sender))
        emit mapped(watcher, d->objectHash.value(sender));
}

QList<QObject*> DBusSignalMapper::intMaps()
{
    Q_D(DBusSignalMapper);
    return d->intHash.keys();
}

QList<QObject*> DBusSignalMapper::stringMaps()
{
    Q_D(DBusSignalMapper);
    return d->stringHash.keys();
}

QList<QObject*> DBusSignalMapper::widgetMaps()
{
    Q_D(DBusSignalMapper);
    return d->widgetHash.keys();
}

QList<QObject*> DBusSignalMapper::objectMaps()
{
    Q_D(DBusSignalMapper);
    return d->objectHash.keys();
}


} // dbus

#include "moc_signal_mapper.cpp"
