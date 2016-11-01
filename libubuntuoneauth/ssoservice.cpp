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
#include <fcntl.h>
#include <glib/gstdio.h>
#include <snapd-glib/snapd-glib.h>
#include <sys/utsname.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QtGlobal>
#include <QNetworkRequest>
#include <QUrlQuery>

#include "logging.h"
#include "ssoservice.h"
#include "requests.h"
#include "errormessages.h"

namespace UbuntuOne {

    static Token _pendingPing;

    SSOService::SSOService(QObject *parent) :
        QObject(parent),
        _snapdAuthPath(QDir::homePath() + "/.snap/auth.json")
    {
        // Set up logging
        AuthLogger::setupLogging();
#if ENABLE_DEBUG
        AuthLogger::setLogLevel(QtDebugMsg);
#else
        if (qgetenv("U1_DEBUG") != "")
            AuthLogger::setLogLevel(QtDebugMsg);
#endif

        // create the keyring that will be used to store and retrieve the different tokens
        _keyring = new Keyring(this);
        _nam = new QNetworkAccessManager(this);

        connect(_keyring, SIGNAL(tokenFound(const Token&)),
                this, SLOT(handleCredentialsFound(const Token&)));
        connect(_keyring, SIGNAL(tokenNotFound()),
                this, SLOT(handleCredentialsNotFound()));

        connect(_keyring, SIGNAL(tokenStored()),
                this, SLOT(handleTokenStored()));
        connect(_keyring, SIGNAL(tokenDeleted()),
                this, SLOT(handleTokenDeleted()));

        connect(_keyring, SIGNAL(keyringError(QString)),
                this, SLOT(handleKeyringError(QString)));

        connect(_nam, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(accountPinged(QNetworkReply*)));

        connect(&(_provider),
                SIGNAL(OAuthTokenGranted(const OAuthTokenResponse&)),
                this, SLOT(tokenReceived(const OAuthTokenResponse&)));
        connect(&(_provider),
                SIGNAL(AccountGranted(const AccountResponse&)),
                this, SLOT(accountRegistered(const AccountResponse&)));
        connect(&(_provider),
                SIGNAL(TwoFactorAuthRequired()),
                this, SLOT(handleTwoFactorAuthRequired()));
        connect(&(_provider),
                SIGNAL(ErrorOccurred(const ErrorResponse&)),
                this, SLOT(errorOccurred(const ErrorResponse&)));
    }

    void SSOService::getCredentials()
    {
        _keyring->findToken();
    }

    void SSOService::handleCredentialsNotFound()
    {
        emit credentialsNotFound();
    }
    
    void SSOService::handleCredentialsFound(const Token& token)
    {
        emit credentialsFound(token);
    }

    void SSOService::handleTokenStored()
    {
        emit credentialsStored(); 
    }

    void SSOService::registerUser(QString email, QString password,
                                  QString display_name)
    {
        AccountRequest request(getAuthBaseUrl(),
                               email, password, display_name);

        _tempPassword = password;
        _tempEmail = email;
        _provider.CreateAccount(request);
    }

    void SSOService::accountRegistered(const AccountResponse& account)
    {
        login(_tempEmail, _tempPassword);
        _tempPassword = "";
    }

    void SSOService::login(QString email, QString password, QString twoFactorCode)
    {
        GError *error = nullptr;
        auto snapdAuth = snapd_login_sync(email.toStdString().c_str(),
                                          password.toStdString().c_str(),
                                          twoFactorCode.toStdString().c_str(),
                                          nullptr, &error);
        if (error != nullptr) {
            qDebug() << "Login failed:" << error->message;
            ErrorResponse rsp{500, "", "", error->message};
            emit errorOccurred(rsp);
            g_clear_error(&error);
            return;
        } else if (snapdAuth == nullptr) {
            ErrorResponse rsp{500, "", "",
                    "An unspecified error occurred while logging in to snapd."
                    };
            emit errorOccurred(rsp);
            return;
        } else {
            qDebug() << "Successful login. Attempting to write ~/.snap/auth.json";
            auto cpath = _snapdAuthPath.toStdString().c_str();
            auto dirpath = g_path_get_dirname(cpath);
            auto result = g_mkdir_with_parents(dirpath, 0700);
            auto errnum = result == 0 ? 0 : errno;
            qDebug() << "Finished mkdir of:" << dirpath << "result:" << errnum;
            g_free(dirpath);

            if (errnum != 0) {
                auto errorString = strerror(errnum);
                ErrorResponse rsp{500, "", "", errorString};
                emit errorOccurred(rsp);
                free(errorString);
                return;
            }

            auto jsonDischarges = g_strjoinv("\",\"", snapd_auth_data_get_discharges(snapdAuth));
            auto jsonOutput = g_strdup_printf("{\"macaroon\":\"%s\",\"discharges\":[\"%s\"]}",
                                              snapd_auth_data_get_macaroon(snapdAuth),
                                              jsonDischarges);
            g_file_set_contents(cpath, jsonOutput,
                                g_utf8_strlen(jsonOutput, -1), &error);
            g_free (jsonDischarges);
            g_free (jsonOutput);

            if (g_file_test(cpath, G_FILE_TEST_EXISTS)) {
                g_chmod(cpath, 0600);
            }

            if (error != nullptr) {
                ErrorResponse rsp{500, "", "", error->message};
                emit errorOccurred(rsp);
                g_clear_error(&error);
                return;
            }
        }
        OAuthTokenRequest request(getAuthBaseUrl(),
                                  email, password,
                                  Token::buildTokenName(), twoFactorCode);
        _tempEmail = email;

        _provider.GetOAuthToken(request);
    }

    void SSOService::handleTwoFactorAuthRequired()
    {
        emit twoFactorAuthRequired();
    }

    QString SSOService::getAuthBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_AUTH_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://login.ubuntu.com");
        return baseUrl;
    }

    QString SSOService::getUOneBaseUrl()
    {
        QString baseUrl = qgetenv("SSO_UONE_BASE_URL");
        if (baseUrl.isEmpty())
            baseUrl = QStringLiteral("https://one.ubuntu.com");
        return baseUrl;
    }

    void SSOService::tokenReceived(const OAuthTokenResponse& token)
    {
        Token realToken = Token(token.token_key(), token.token_secret(),
                                token.consumer_key(), token.consumer_secret(),
                                token.date_created(), token.date_updated());
        _keyring->storeToken(realToken, _tempEmail);
    }

    void SSOService::accountPinged(QNetworkReply*)
    {
        /* DEPRECATED */
    }

    void SSOService::invalidateCredentials()
    {
        QFile::remove(_snapdAuthPath);
        _keyring->deleteToken();
    }

    void SSOService::errorOccurred(const ErrorResponse& error)
    {
        _tempPassword = "";
        emit requestFailed(error);
    }

    void SSOService::handleKeyringError(QString message)
    {
        _tempPassword = "";
        /* Treat keyring errors as invalid/expired token.
           Delete the token and act like it wasn't found. */
        invalidateCredentials();
        emit credentialsNotFound();
    }
}
