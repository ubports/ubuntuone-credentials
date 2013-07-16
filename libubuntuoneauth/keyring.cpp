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
#include <libsecret/secret.h>

#include <QDebug>

#include "keyring.h"

#define TOKEN_KEY_TYPE "Ubuntu SSO credentials"
#define TOKEN_ATTR_TYPE_KEY "key-type"
#define TOKEN_ATTR_NAME_KEY "token-name"

namespace UbuntuOne {

    Keyring::Keyring(QObject *parent)
        : QObject(parent)
    {
    }

    static const SecretSchema *_getTokenSchema()
    {
        static const SecretSchema schema = {
            "com.ubuntu.one.Token", SECRET_SCHEMA_DONT_MATCH_NAME,
            {
                { TOKEN_ATTR_TYPE_KEY, SECRET_SCHEMA_ATTRIBUTE_STRING },
                { TOKEN_ATTR_NAME_KEY, SECRET_SCHEMA_ATTRIBUTE_STRING },
            }
        };

        return &schema;
    }

    static void _onPasswordLookup(GObject *source, GAsyncResult *result,
                                  Keyring *keyring)
    {
        GError *error = NULL;

        gchar *password = secret_password_lookup_finish(result, &error);

        if (error != NULL) {
            QString message(error->message);
            qCritical() << "Error in secret_password_lookup: " <<  message;
            emit keyring->keyringError(message);
            g_error_free(error);
        } else if (password == NULL) {
            emit keyring->tokenNotFound();
        } else {
            QString query(password);
            Token *token = Token::fromQuery(query);
            if (token->isValid()) {
                emit keyring->tokenFound(*token);
            } else {
                QString message("Faild to convert result to Token object.");
                qCritical() << message;
                emit keyring->keyringError(message);
            }
            delete token;
            secret_password_free(password);
        }
    }

    void Keyring::findToken()
    {
        secret_password_lookup(_getTokenSchema(), NULL,
                               (GAsyncReadyCallback)_onPasswordLookup, this,
                               TOKEN_ATTR_TYPE_KEY, TOKEN_KEY_TYPE,
                               TOKEN_ATTR_NAME_KEY, Token::buildTokenName().toUtf8().data(),
                               NULL);
    }

    static void _onPasswordStored(GObject *source, GAsyncResult *result,
                                  Keyring *keyring)
    {
        GError *error = NULL;

        secret_password_store_finish (result, &error);
        if (error != NULL) {
            QString message(error->message);
            qCritical() << "Error storing token: " << message;
            emit keyring->keyringError(message);
            g_error_free(error);
            return;
        }
        emit keyring->tokenStored();
    }

    void Keyring::storeToken(Token token)
    {
        secret_password_store(_getTokenSchema(), SECRET_COLLECTION_DEFAULT,
                              TOKEN_ID, token.toQuery().toUtf8().data(), NULL,
                              (GAsyncReadyCallback)_onPasswordStored, this,
                              TOKEN_ATTR_TYPE_KEY, TOKEN_KEY_TYPE,
                              TOKEN_ATTR_NAME_KEY, Token::buildTokenName().toUtf8().data(),
                              NULL);
    }

    static void _onPasswordCleared(GObject *source, GAsyncResult *result,
                                   Keyring *keyring)
    {
        GError *error = NULL;

        secret_password_clear_finish (result, &error);
        if (error != NULL) {
            QString message(error->message);
            qDebug() << message;
            emit keyring->keyringError(message);
            g_error_free(error);
        } else {
            emit keyring->tokenDeleted();
        }
    }

    void Keyring::deleteToken()
    {
        secret_password_clear(_getTokenSchema(), NULL,
                              (GAsyncReadyCallback)_onPasswordCleared, this,
                              TOKEN_ATTR_TYPE_KEY, TOKEN_KEY_TYPE,
                              TOKEN_ATTR_NAME_KEY, Token::buildTokenName().toUtf8().data(),
                              NULL);
    }

} // namespace UbuntuOne
