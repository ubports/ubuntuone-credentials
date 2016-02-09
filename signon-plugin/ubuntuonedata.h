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
#ifndef _UBUNTUONE_DATA_H_
#define _UBUNTUONE_DATA_H_

#include <sessiondata.h>

namespace UbuntuOne {

    class PluginData : public SignOn::SessionData
    {
    public:
        PluginData(const QVariantMap &data = QVariantMap()):
            SignOn::SessionData(data) {}

        // The name of the token
        SIGNON_SESSION_DECLARE_PROPERTY(QString, TokenName);

        // The one-time password (optional)
        SIGNON_SESSION_DECLARE_PROPERTY(QString, OneTimePassword);

        // The consumer key and secret for signing
        SIGNON_SESSION_DECLARE_PROPERTY(QString, ConsumerKey);
        SIGNON_SESSION_DECLARE_PROPERTY(QString, ConsumerSecret);

        // The access token and secret for signing
        SIGNON_SESSION_DECLARE_PROPERTY(QString, TokenKey);
        SIGNON_SESSION_DECLARE_PROPERTY(QString, TokenSecret);

        // Data which the plugin has stored into signond
        SIGNON_SESSION_DECLARE_PROPERTY(QVariantMap, StoredData);
    };

} // namespace UbuntuOne

#endif
