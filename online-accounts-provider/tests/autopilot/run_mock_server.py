# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Mock server for SSO and U1 Ping endpoints."""

import BaseHTTPServer
import json
import signal
import sys

from urlparse import urlparse, parse_qs

SSO_API_PATH = '/api/v2'
UONE_API_PATH = '/oauth/sso-finished-so-get-tokens/'


LOGIN_OK_RESPONSE = """
{
    "href": "https://login.ubuntu.com/api/v2/tokens/oauth/the-key",
    "token_key": "token-key",
    "token_secret": "token-secret",
    "token_name": "token-name",
    "consumer_key": "consumer-key",
    "consumer_secret": "consumer-secret",
    "date_created": "2013-01-11 12:43:23",
    "date_updated": "2013-01-11 12:43:23"
}
"""

TWOFACTOR_REQUIRED_RESPONSE = """
{
    "code": "TWOFACTOR_REQUIRED",
    "message": "This account requires 2-factor authentication.",
    "extra": {}
}
"""

LOGIN_ERR_RESPONSE = """
{
    "code": "INVALID_CREDENTIALS",
    "message": "Your email/password isn't correct.",
    "extra": {}
}
"""

TWOFACTOR_ERR_RESPONSE = """
{
    "message": "The provided 2-factor key is not recognised.",
    "code": "TWOFACTOR_FAILURE",
    "extra": {}
}
"""

INVALID_DATA_RESPONSE = """
{
    "message": "Invalid request data",
    "code": "INVALID_DATA",
    "extra": {
        "password": [
            "A real password-specific message would go here."
        ],
        "email": [
            "A real email-specific message would go here."
        ]
    }
}
"""

REGISTER_CREATED_RESPONSE = """
{
    "href": "https://login.ubuntu.com/api/v2/accounts/openid123",
    "openid": "https://login.ubuntu.com/+id/openid123",
    "preferredemail": "newaccount@te.st",
    "displayname": "New Test Mock Account",
    "status": "NEW",
    "verified": False,
    "emails": [
        {
            "href": "https://login.ubuntu.com/api/v2/emails/newaccount@te.st",
        }
    ]
}
"""


class MockSSOAndUOneRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def do_GET(self):
        """Handle GET as if we were the one.ubuntu.com server
        """
        parsedPath = urlparse(self.path)
        queryDict = parse_qs(parsedPath.query)
        print "IN GET:"
        print " path: ", self.path
        print " headers: ", self.headers

        if parsedPath.path.startswith('/oauth/sso-finished-so-get-tokens/'):
            for k in ['platform', 'platform_version', 'platform_arch']:
                if k not in queryDict:
                    print "****** MISSING ", k, " IN ", queryDict , "bailing"
                    self.send_error(401)
                    return
            self.send_response(200)
            self.end_headers()
            self.wfile.write("ok 0/0")  # fake format of server's real response
        else:
            self.send_error(404)  # real server currently returns 500!

    def do_POST(self):
        """Handle POST as if we are the login.ubuntu.com server.
        """
        length = int(self.headers['Content-Length'])
        postdata = self.rfile.read(length)
        if postdata == '':
            print "NOTE: empty postdata, just dropping the request"
            return
        print "IN POST:"
        print " path:", self.path
        print " headers:", self.headers
        print " postdata: '%s'" % postdata

        try:
            bodyDict = json.loads(postdata)
        except ValueError:
            print "error loading json from postdata: '%s'" % postdata
            self.send_error(500)
            return

        if self.path == SSO_API_PATH + '/tokens/oauth':
            self.handle_login(bodyDict)
        elif self.path == SSO_API_PATH + '/accounts':
            self.handle_register(bodyDict)
        else:
            print "unknown -- self.path is ", self.path
            self.send_error(404, 'unknown')

    def send_JSON_reply(self, code, replyJSON):
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(replyJSON)

    def handle_login(self, bodyDict):

        if 'email' not in bodyDict \
                or 'password' not in bodyDict or 'token_name' not in bodyDict:
            self.send_JSON_reply(400, INVALID_DATA_RESPONSE)
            return

        email = bodyDict['email']

        if email == "ok@te.st":
            self.send_JSON_reply(201, LOGIN_OK_RESPONSE)

        elif email == "2fa@te.st":
            if 'otp' not in bodyDict:
                self.send_JSON_reply(401, TWOFACTOR_REQUIRED_RESPONSE)
            else:
                if len(bodyDict['otp']) != 6:
                    self.send_JSON_reply(403, TWOFACTOR_ERR_RESPONSE)
                else:
                    self.send_JSON_reply(201, LOGIN_OK_RESPONSE)

        else:
            self.send_JSON_reply(401, LOGIN_ERR_RESPONSE)

    def handle_register(self, bodyDict):
        if 'email' not in bodyDict \
                or 'password' not in bodyDict or 'token_name' not in bodyDict:
            self.send_JSON_reply(400, INVALID_DATA_RESPONSE)
            return

        self.send_JSON_reply(201, REGISTER_CREATED_RESPONSE)


if __name__ == '__main__':

    server_address = ('', 8080)
    httpd = BaseHTTPServer.HTTPServer(server_address,
                                      MockSSOAndUOneRequestHandler)

    def stop_immediately(signum, frame):
        print "Got signal ", signum, ", stopping server."
        httpd.socket.close()

    signal.signal(signal.SIGINT, stop_immediately)
    signal.signal(signal.SIGHUP, stop_immediately)

    print "Mock server running at ", 8080

    try:
        httpd.serve_forever()
    except:
        pass
