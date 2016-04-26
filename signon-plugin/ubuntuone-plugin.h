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

class PluginTest;

class QNetworkAccessManager;
class QNetworkReply;

namespace UbuntuOne {

    class Token;

    class SignOnPlugin : public AuthPluginInterface
    {
        Q_OBJECT
        Q_INTERFACES(AuthPluginInterface)

    public:
        SignOnPlugin(QObject *parent = 0);
        virtual ~SignOnPlugin();

    public Q_SLOTS:
        QString type() const Q_DECL_OVERRIDE;
        QStringList mechanisms() const Q_DECL_OVERRIDE;
        void cancel() Q_DECL_OVERRIDE;
        void process(const SignOn::SessionData &inData,
                     const QString &mechanism = 0) Q_DECL_OVERRIDE;
        void userActionFinished(const SignOn::UiSessionData &data) Q_DECL_OVERRIDE;

    private:
        bool validateInput(const PluginData &data, const QString &mechanism);
        bool respondWithStoredData();
        void emitErrorFromReply(QNetworkReply *reply);
        void createNewToken();
        void getCredentialsAndCreateNewToken();
        bool handleUiError(const SignOn::UiSessionData &data);

    private Q_SLOTS:
        void onCreationFinished();

    private:
        friend class ::PluginTest;
        PluginData m_data;
        QNetworkAccessManager *m_networkAccessManager;
        QNetworkReply *m_reply;
        bool m_didAskForPassword;
        bool m_needsOtp;
    };

} // namespace UbuntuOne

/* These fields are temporarily defined here; they'll be eventually moved to
 * signond's include files. */
#define SSOUI_KEY_USERNAME_TEXT QLatin1String("UserNameText")
#define SSOUI_KEY_PASSWORD_TEXT QLatin1String("PasswordText")
#define SSOUI_KEY_REGISTER_URL  QLatin1String("RegisterUrl")
#define SSOUI_KEY_REGISTER_TEXT QLatin1String("RegisterText")
#define SSOUI_KEY_LOGIN_TEXT QLatin1String("LoginText")
#define SSOUI_KEY_QUERY2FA QLatin1String("Query2fa")
#define SSOUI_KEY_2FA QLatin1String("2fa")
#define SSOUI_KEY_2FA_TEXT QLatin1String("2faText")
#define SSOUI_KEY_ERROR_MESSAGE QLatin1String("ErrorMessage")

#endif
