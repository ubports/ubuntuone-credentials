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
#ifndef _UBUNTUONE_PLUGIN_H_
#define _UBUNTUONE_PLUGIN_H_

#include <QtCore>

#include <SignOn/AuthPluginInterface>
#include <SignOn/Error>
#include <SignOn/SessionData>
#include <SignOn/UiSessionData>

#include "ubuntuonedata.h"


namespace UbuntuOne {

    class SignOnPlugin : public AuthPluginInterface
    {
        Q_OBJECT
        Q_INTERFACES(AuthPluginInterface)

    public:
        SignOnPlugin(QObject *parent = 0);
        virtual ~SignOnPlugin();

    public Q_SLOTS:
        QString type() const;
        QStringList mechanisms() const;
        void cancel();
        void process(const SignOn::SessionData &inData,
                     const QString &mechanism = 0);
        void userActionFinished(const SignOn::UiSessionData &data);

    private:
        PluginData m_data;
    };

} // namespace UbuntuOne

#endif
