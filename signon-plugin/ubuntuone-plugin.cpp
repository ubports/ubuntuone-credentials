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
#include <token.h>

#include "ubuntuone-plugin.h"


namespace UbuntuOne {

    SignOnPlugin::SignOnPlugin(QObject *parent)
        : AuthPluginInterface(parent)
    {
    }

    SignOnPlugin::~SignOnPlugin()
    {
    }

    QString SignOnPlugin::type() const
    {
        return QLatin1String("ubuntuone");
    }

    QStringList SignOnPlugin::mechanisms() const
    {
        QStringList res = QStringList(QLatin1String("ubuntuone"));

        return res;
    }

    void SignOnPlugin::cancel()
    {
    }

    void SignOnPlugin::process(const SignOn::SessionData &inData,
                               const QString &mechanism)
    {
        Q_UNUSED(mechanism);
        PluginData response;
        m_data = inData.data<PluginData>();

        if (!inData.Secret().isEmpty()) {
            response.setConsumer(m_data.Consumer());
            response.setConsumerSecret(m_data.ConsumerSecret());
            response.setToken(m_data.Token());
            response.setTokenSecret(m_data.TokenSecret());

            response.setName(Token::buildTokenName());

            emit result(response);
            return;
        }

        SignOn::UiSessionData data;
        data.setRealm(inData.Realm());
        data.setShowRealm(!data.Realm().isEmpty());
        emit userActionRequired(data);
    }

    void SignOnPlugin::userActionFinished(const SignOn::UiSessionData &data)
    {
    }

    SIGNON_DECL_AUTH_PLUGIN(SignOnPlugin)
} // namespace UbuntuOne
