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

#include "keyring_signal_mapper.h"


namespace keyring
{

KeyringSignalMapper::_init KeyringSignalMapper::_initializer;

class KeyringSignalMapperPrivate
{
    Q_DECLARE_PUBLIC(KeyringSignalMapper)
 public:
    KeyringSignalMapperPrivate(KeyringSignalMapper* parent) :
        q_ptr(parent) {}

    void _q_senderDestroyed() {
        Q_Q(KeyringSignalMapper);
        q->removeMappings(q->sender());
    }
    QHash<QObject*, int> intHash;
    QHash<QObject*, QString> stringHash;
    QHash<QObject*, QWidget*> widgetHash;
    QHash<QObject*, QObject*> objectHash;

private:
    KeyringSignalMapper* q_ptr;
};

KeyringSignalMapper::KeyringSignalMapper(QObject *parent) :
    QObject(parent),
    d_ptr(new KeyringSignalMapperPrivate(this))
{
}

KeyringSignalMapper::~KeyringSignalMapper()
{
}

void KeyringSignalMapper::setMapping(QObject* sender, int id)
{
    Q_D(KeyringSignalMapper);
    d->intHash.insert(sender, id);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KeyringSignalMapper::setMapping(QObject* sender, const QString &text)
{
    Q_D(KeyringSignalMapper);
    d->stringHash.insert(sender, text);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KeyringSignalMapper::setMapping(QObject* sender, QWidget *widget)
{
    Q_D(KeyringSignalMapper);
    d->widgetHash.insert(sender, widget);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

void KeyringSignalMapper::setMapping(QObject* sender, QObject *object)
{
    Q_D(KeyringSignalMapper);
    d->objectHash.insert(sender, object);
    connect(sender, SIGNAL(destroyed()), this, SLOT(_q_senderDestroyed()));
}

QObject* KeyringSignalMapper::mapping(int id) const
{
    Q_D(const KeyringSignalMapper);
    return d->intHash.key(id);
}

QObject* KeyringSignalMapper::mapping(const QString& id) const
{
    Q_D(const KeyringSignalMapper);
    return d->stringHash.key(id);
}

QObject* KeyringSignalMapper::mapping(QWidget* widget) const
{
    Q_D(const KeyringSignalMapper);
    return d->widgetHash.key(widget);
}

QObject* KeyringSignalMapper::mapping(QObject* object) const
{
    Q_D(const KeyringSignalMapper);
    return d->objectHash.key(object);
}

void KeyringSignalMapper::removeMappings(QObject* sender)
{
    Q_D(KeyringSignalMapper);

    d->intHash.remove(sender);
    d->stringHash.remove(sender);
    d->widgetHash.remove(sender);
    d->objectHash.remove(sender);
}

void KeyringSignalMapper::map(bool dismissed, const QDBusVariant result)
{
    map(dismissed, result, sender());
}

void KeyringSignalMapper::map(bool dismissed, const QDBusVariant result, QObject* sender)
{
    Q_D(KeyringSignalMapper);
    if (d->intHash.contains(sender))
        emit mapped(dismissed, result, d->intHash.value(sender));
    if (d->stringHash.contains(sender))
        emit mapped(dismissed, result, d->stringHash.value(sender));
    if (d->widgetHash.contains(sender))
        emit mapped(dismissed, result, d->widgetHash.value(sender));
    if (d->objectHash.contains(sender))
        emit mapped(dismissed, result, d->objectHash.value(sender));
}

QList<QObject*> KeyringSignalMapper::intMaps()
{
    Q_D(KeyringSignalMapper);
    return d->intHash.keys();
}

QList<QObject*> KeyringSignalMapper::stringMaps()
{
    Q_D(KeyringSignalMapper);
    return d->stringHash.keys();
}

QList<QObject*> KeyringSignalMapper::widgetMaps()
{
    Q_D(KeyringSignalMapper);
    return d->widgetHash.keys();
}

QList<QObject*> KeyringSignalMapper::objectMaps()
{
    Q_D(KeyringSignalMapper);
    return d->objectHash.keys();
}


} // keyring

#include "moc_keyring_signal_mapper.cpp"
