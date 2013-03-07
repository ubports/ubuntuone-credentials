/**
 *
 * Copyright 2013 Canonical Ltd.
 * Author: Diego Sarmentero <diego.sarmentero@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include <QCoreApplication>

#include "sso_api/requests.h"
#include "sso_api/identityprovider.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SSO::IdentityProvider provider;

    /*
    SSO::PasswordTokenRequest request(getenv("EMAIL"));
    provider.GetPasswordToken(request);
    */

    SSO::OAuthTokenRequest request(getenv("EMAIL"), getenv("PASSWORD"),
                                   "sso-test",
                                   NULL);
                                   //"incorrectotp");
    provider.GetOAuthToken(request);

    /*
    SSO::AccountRequest request(getenv("EMAIL"), getenv("PASSWORD"), "brian",
                                NULL, NULL, true);
    provider.CreateAccount(request);
    */

    return a.exec();
}
