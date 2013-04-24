/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEYRING_SIGNAL_MAPPER_H
#define KEYRING_SIGNAL_MAPPER_H

#include <QDBusVariant>
#include <QObject>


namespace keyring
{

// this is inspired by the QSignalMapper class
class KeyringSignalMapperPrivate;
class KeyringSignalMapper : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(KeyringSignalMapper)
public:
    explicit KeyringSignalMapper(QObject* parent = 0);
    ~KeyringSignalMapper();

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
                qRegisterMetaType<QDBusVariant>("QDBusVariant");
            }
    } _initializer;

Q_SIGNALS:
    void mapped(bool dismissed, const QDBusVariant result, int id);
    void mapped(bool dismissed, const QDBusVariant result, const QString& text);
    void mapped(bool dismissed, const QDBusVariant result, QWidget* widget);
    void mapped(bool dismissed, const QDBusVariant result, QObject* object);

public Q_SLOTS:
    void map(bool dismissed, const QDBusVariant result);
    void map(bool dismissed, const QDBusVariant result, QObject* sender);

protected:
    QScopedPointer<KeyringSignalMapperPrivate> d_ptr;
    QList<QObject*> intMaps();
    QList<QObject*> stringMaps();
    QList<QObject*> widgetMaps();
    QList<QObject*> objectMaps();

private:
    Q_DISABLE_COPY(KeyringSignalMapper)
    Q_PRIVATE_SLOT(d_func(), void _q_senderDestroyed())
};

} // keyring

#endif // KEYRING_SIGNAL_MAPPER_H
