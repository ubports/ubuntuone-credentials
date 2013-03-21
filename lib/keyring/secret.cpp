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

#include "secret.h"


Secret::Secret() :
    _session(),
    _parameters(),
    _value(),
    _contentType()
{
}

Secret::Secret(QDBusObjectPath session, QByteArray parameters, QByteArray value, QString contentType) :
    _session(session),
    _parameters(parameters),
    _value(value),
    _contentType(contentType)
{
}

Secret::Secret(const Secret& other) :
    _session(other._session),
    _parameters(other._parameters),
    _value(other._value),
    _contentType(other._contentType)
{
}

Secret& Secret::operator=(const Secret& other)
{
    _session = other._session;
    _parameters = other._parameters;
    _value = other._value;
    _contentType = other._contentType;

    return *this;
}

Secret::~Secret()
{
}

void Secret::registerMetaType()
{
    qRegisterMetaType<Secret>("Secret");
    qDBusRegisterMetaType<Secret>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const Secret& secret)
{
    argument.beginStructure();
    argument << secret._session;
    argument << secret._parameters;
    argument << secret._value;
    argument << secret._contentType;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Secret &secret)
{
    argument.beginStructure();
    argument >> secret._session;
    argument >> secret._parameters;
    argument >> secret._value;
    argument >> secret._contentType;
    argument.endStructure();

    return argument;
}

QString Secret::getSession() const
{
    return _session.path();
}

QByteArray Secret::getParameters() const
{
    return _parameters;
}

QByteArray Secret::getValue() const
{
    return _value;
}

QString Secret::getContentType() const
{
    return _contentType;
}
