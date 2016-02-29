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

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <SignOn/UiSessionData>
#include <SignOn/uisessiondata_priv.h>

#include <token.h>

#include "i18n.h"
#include "ubuntuone-plugin.h"

#define BASE_URL "https://login.ubuntu.com"

#define ERR_INVALID_CREDENTIALS QLatin1String("INVALID_CREDENTIALS")
#define ERR_TWOFACTOR_REQUIRED QLatin1String("TWOFACTOR_REQUIRED")
#define ERR_TWOFACTOR_FAILURE QLatin1String("TWOFACTOR_FAILURE")
#define ERR_PASSWORD_POLICY_ERROR QLatin1String("PASSWORD_POLICY_ERROR")

namespace UbuntuOne {

    SignOnPlugin::SignOnPlugin(QObject *parent):
        AuthPluginInterface(parent),
        m_networkAccessManager(0),
        m_reply(0),
        m_didAskForPassword(false),
        m_needsOtp(false)
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

    bool SignOnPlugin::validateInput(const PluginData &data,
                                     const QString &mechanism)
    {
        Q_UNUSED(mechanism);

        if (data.TokenName().isEmpty()) {
            return false;
        }

        return true;
    }

    bool SignOnPlugin::respondWithStoredData()
    {
        QVariantMap storedData = m_data.StoredData();

        /* When U1 was using the password plugin, it was storing the token data
         * in the password field. So, if we don't have any data stored in the
         * plugin's data, try to get a token from the password field.
         */
        if (storedData.isEmpty() && !m_data.Secret().isEmpty()) {
            Token *token = Token::fromQuery(m_data.Secret());
            if (token->isValid()) {
                PluginData tokenData;
                tokenData.setConsumerKey(token->consumerKey());
                tokenData.setConsumerSecret(token->consumerSecret());
                tokenData.setTokenKey(token->tokenKey());
                tokenData.setTokenSecret(token->tokenSecret());
                storedData[token->name()] = tokenData.toMap();
                PluginData pluginData;
                pluginData.setStoredData(storedData);
                Q_EMIT store(pluginData);

                /* We know that the given secret is a valid token, so it cannot
                 * be a valid password as well: let's clear it out now, so that
                 * if it turns out that the token is no longer valid and that
                 * we need to create a new one, we won't make a useless attempt
                 * to create one with a wrong password.
                 */
                m_data.setSecret(QString());
            }
            delete token;
        }

        /* Check if we have stored data for this token name */
        PluginData tokenData(storedData[m_data.TokenName()].toMap());
        Token token(tokenData.TokenKey(), tokenData.TokenSecret(),
                    tokenData.ConsumerKey(), tokenData.ConsumerSecret());
        if (!token.isValid()) return false;
        qDebug() << "Token is valid!" << tokenData.TokenKey();

        tokenData.setTokenName(m_data.TokenName());
        checkTokenValidity(token, tokenData);
        return true;
    }

    void SignOnPlugin::emitErrorFromReply(QNetworkReply *reply)
    {
        int errorCode = reply->error();
        int type = SignOn::Error::Network;
        if (errorCode == QNetworkReply::SslHandshakeFailedError) {
            type = SignOn::Error::Ssl;
        } else if (errorCode == QNetworkReply::ServiceUnavailableError) {
            type = SignOn::Error::ServiceNotAvailable;
        } else if (errorCode == QNetworkReply::AuthenticationRequiredError) {
            type = SignOn::Error::NotAuthorized;
        } else if (errorCode <= QNetworkReply::UnknownNetworkError) {
            type = SignOn::Error::NoConnection;
        }

        qDebug() << "Got error:" << reply->errorString();
        Q_EMIT error(SignOn::Error(type, reply->errorString()));
    }

    void SignOnPlugin::onValidationFinished()
    {
        /* Note on error handling: we consider the token to be (in)valid only if
         * we can parse the JSON response and it contains the response.  If the
         * validation has failed because of some other error (SSL error,
         * unparsable server reply, etc.) then we report the error to the
         * client.
         */
        QJsonDocument json = QJsonDocument::fromJson(m_reply->readAll());
        QJsonObject object = json.object();
        QJsonValue value = object.value("is_valid");

        int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200 && value.isBool()) {
            bool isValid = value.toBool();
            if (isValid) {
                Q_EMIT result(m_checkedToken);
            } else {
                getCredentialsAndCreateNewToken();
            }
        } else {
            emitErrorFromReply(m_reply);
        }

        m_reply->deleteLater();
        m_reply = 0;
    }

    void SignOnPlugin::checkTokenValidity(const Token &token,
                                          const PluginData &tokenData)
    {
        m_checkedToken = tokenData;

        QNetworkRequest req(QUrl(BASE_URL "/api/v2/requests/validate"));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString httpUrl("http://www.example.com");
        QString httpMethod("GET");

        QJsonObject formData;
        formData.insert("http_url", httpUrl);
        formData.insert("http_method", httpMethod);
        formData.insert("authorization", token.signUrl(httpUrl, httpMethod));

        m_reply =
            m_networkAccessManager->post(req, QJsonDocument(formData).toJson());
        QObject::connect(m_reply, SIGNAL(finished()),
                         this, SLOT(onValidationFinished()));
    }

    void SignOnPlugin::process(const SignOn::SessionData &inData,
                               const QString &mechanism)
    {
        if (!m_networkAccessManager) {
            m_networkAccessManager = new QNetworkAccessManager(this);
        }

        initTr("ubuntuone-credentials", NULL);

        PluginData response;
        m_data = inData.data<PluginData>();

        if (!validateInput(m_data, mechanism)) {
            qWarning() << "Invalid parameters passed";
            Q_EMIT error(SignOn::Error(SignOn::Error::MissingData));
            return;
        }

        /* It may be that the stored token is valid; however, do the check only
         * if no OTP was provided (since the presence of an OTP is a clear
         * signal that the caller wants to get a new token). */
        if (m_data.OneTimePassword().isEmpty() &&
            respondWithStoredData()) {
            return;
        }

        getCredentialsAndCreateNewToken();
    }

    void SignOnPlugin::onCreationFinished()
    {
        QNetworkReply *reply = m_reply;
        m_reply->deleteLater();
        m_reply = 0;

        QByteArray data = reply->readAll();
        qDebug() << "Received" << data;
        QJsonDocument json = QJsonDocument::fromJson(data);
        QJsonObject object = json.object();

        QString error = object.value("code").toString();

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200 || statusCode == 201) {
            QString tokenName = object.value("token_name").toString();
            PluginData token;
            token.setConsumerKey(object.value("consumer_key").toString());
            token.setConsumerSecret(object.value("consumer_secret").toString());
            token.setTokenKey(object.value("token_key").toString());
            token.setTokenSecret(object.value("token_secret").toString());

            /* Store the token */
            QVariantMap storedData;
            storedData[tokenName] = token.toMap();
            PluginData pluginData;
            pluginData.setStoredData(storedData);
            Q_EMIT store(pluginData);

            token.setTokenName(tokenName);
            Q_EMIT result(token);
        } else if (statusCode == 401 && error == ERR_INVALID_CREDENTIALS) {
            m_data.setSecret(QString());
            m_data.setOneTimePassword(QString());
            getCredentialsAndCreateNewToken();
        } else if (statusCode == 401 && error == ERR_TWOFACTOR_REQUIRED) {
            m_needsOtp = true;
            getCredentialsAndCreateNewToken();
        } else if (statusCode == 403 && error == ERR_TWOFACTOR_FAILURE) {
            m_data.setOneTimePassword(QString());
            getCredentialsAndCreateNewToken();
        } else if (statusCode == 403 && error == ERR_PASSWORD_POLICY_ERROR) {
            QVariantMap data;
            QJsonObject extra = object.value("extra").toObject();
            data[SSOUI_KEY_OPENURL] = extra.value("location").toString();
            Q_EMIT userActionRequired(data);
        } else {
            emitErrorFromReply(reply);
        }
    }

    void SignOnPlugin::createNewToken()
    {
        QNetworkRequest req(QUrl(BASE_URL "/api/v2/tokens/oauth"));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QJsonObject formData;
        formData.insert("email", m_data.UserName());
        formData.insert("password", m_data.Secret());
        formData.insert("token_name", m_data.TokenName());
        if (!m_data.OneTimePassword().isEmpty()) {
            formData.insert("otp", m_data.OneTimePassword());
        }

        qDebug() << "Sending" << QJsonDocument(formData).toJson();
        m_reply =
            m_networkAccessManager->post(req, QJsonDocument(formData).toJson());
        QObject::connect(m_reply, SIGNAL(finished()),
                         this, SLOT(onCreationFinished()));
    }

    void SignOnPlugin::getCredentialsAndCreateNewToken()
    {
        if (!m_data.Secret().isEmpty() &&
            (!m_needsOtp || !m_data.OneTimePassword().isEmpty())) {
            createNewToken();
        } else if (m_data.Secret().isEmpty()) {
            QVariantMap data;
            data[SSOUI_KEY_QUERYUSERNAME] = true;
            data[SSOUI_KEY_USERNAME] = m_data.UserName();
            data[SSOUI_KEY_QUERYPASSWORD] = true;
            m_didAskForPassword = true;
            Q_EMIT userActionRequired(data);
        } else {
            QVariantMap data;
            data[SSOUI_KEY_USERNAME] = m_data.UserName();
            data[SSOUI_KEY_PASSWORD] = m_data.Secret();
            data[SSOUI_KEY_QUERY2FA] = true;
            data[SSOUI_KEY_2FA_TEXT] = _("2-factor device code");
            Q_EMIT userActionRequired(data);
        }
    }

    bool SignOnPlugin::handleUiError(const SignOn::UiSessionData &data)
    {
        using namespace SignOn;

        int code = data.QueryErrorCode();
        if (code == QUERY_ERROR_NONE) {
            return false;
        }

        qDebug() << "userActionFinished with error: " << code;
        if (code == QUERY_ERROR_CANCELED) {
            Q_EMIT error(Error(Error::SessionCanceled,
                               QLatin1String("Cancelled by user")));
        } else if (code == QUERY_ERROR_NETWORK) {
            Q_EMIT error(Error(Error::Network, QLatin1String("Network error")));
        } else if (code == QUERY_ERROR_SSL) {
            Q_EMIT error(Error(Error::Ssl, QLatin1String("SSL error")));
        } else {
            Q_EMIT error(Error(Error::UserInteraction,
                               QString("userActionFinished error: ")
                               + QString::number(data.QueryErrorCode())));
        }
        return true;
    }

    void SignOnPlugin::userActionFinished(const SignOn::UiSessionData &data)
    {
        if (handleUiError(data)) return;

        PluginData uiData = data.data<PluginData>();
        if (!uiData.UserName().isEmpty()) {
            m_data.setUserName(uiData.UserName());
        }

        if (!uiData.Secret().isEmpty()) {
            m_data.setSecret(uiData.Secret());
        }

        if (!uiData.OneTimePassword().isEmpty()) {
            m_data.setOneTimePassword(uiData.OneTimePassword());
        }

        getCredentialsAndCreateNewToken();
    }

    SIGNON_DECL_AUTH_PLUGIN(SignOnPlugin)
} // namespace UbuntuOne
