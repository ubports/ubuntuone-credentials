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


namespace UbuntuOne {

    Keyring::Keyring()
    {
    }

    static const SecretSchema *_getTokenSchema()
    {
        static const SecretSchema schema = {
            "com.ubuntu.one.Token", SECRET_SCHEMA_DONT_MATCH_NAME,
            {
                { "key-type", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { "name", SECRET_SCHEMA_ATTRIBUTE_STRING },
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
            qDebug() << "Error getting token: " << error->message;
            emit keyring->keyringError(message);
            g_error_free(error);
        } else if (password == NULL) {
            qDebug() << "didn't find a token.";
            emit keyring->tokenNotFound();
        } else {
            QString query(password);
            Token *token = Token::fromQuery(query);
            if (token->isValid()) {
                qDebug() << "Found a valid token.";
                emit keyring->tokenFound(*token);
            } else {
                qDebug() << "Found token is not valid.";
                emit keyring->keyringError(QStringLiteral("Faild to convert result to Token object."));
            }
            secret_password_free(password);
        }
    }

    void Keyring::findToken()
    {
        secret_password_lookup(_getTokenSchema(), NULL,
                               (GAsyncReadyCallback)_onPasswordLookup, this,
                               "key-type", TOKEN_KEY_TYPE,
                               "name", Token::buildTokenName().toUtf8().data(),
                               NULL);
    }

    static void _onPasswordStored(GObject *source, GAsyncResult *result,
                                  Keyring *keyring)
    {
        GError *error = NULL;

        secret_password_store_finish (result, &error);
        if (error != NULL) {
            QString message(error->message);
            emit keyring->keyringError(message);
            g_error_free(error);
        }
    }

    void Keyring::storeToken(Token token)
    {
        secret_password_store(_getTokenSchema(), SECRET_COLLECTION_DEFAULT,
                              TOKEN_ID, token.toQuery().toUtf8(), NULL,
                              (GAsyncReadyCallback)_onPasswordStored, NULL,
                              "key-type", TOKEN_KEY_TYPE,
                              "name", Token::buildTokenName().toUtf8().data(),
                              NULL);
    }

    static void _onPasswordCleared(GObject *source, GAsyncResult *result,
                                   Keyring *keyring)
    {
        GError *error = NULL;

        secret_password_store_finish (result, &error);
        if (error != NULL) {
            QString message(error->message);
            emit keyring->keyringError(message);
            g_error_free(error);
        }
    }

    void Keyring::deleteToken()
    {
        secret_password_clear(_getTokenSchema(), NULL,
                              (GAsyncReadyCallback)_onPasswordCleared, NULL,
                              "key-type", TOKEN_KEY_TYPE,
                              "name", Token::buildTokenName().toUtf8().data(),
                              NULL);
    }

} // namespace UbuntuOne
