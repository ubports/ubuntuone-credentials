# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

import BaseHTTPServer
import json
import logging
import urlparse


logger = logging.getLogger(__name__)


class FakeSSOAndU1Server(BaseHTTPServer.HTTPServer, object):

    def __init__(self, server_address):
        super(FakeSSOAndU1Server, self).__init__(
            server_address, FakeSSOAndU1RequestHandler)


class FakeSSOAndU1RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    SSO_API_PATH = '/api/v2'

    INVALID_DATA_RESPONSE = {
        'message': 'Invalid request data',
        'code': 'INVALID_DATA',
        'extra': {
            'password': ['A real password-specific message would go here.'],
            'email': ['A real email-specific message would go here.']
        }
    }
    LOGIN_OK_RESPONSE = {
        'href': 'https://login.ubuntu.com/api/v2/tokens/oauth/the-key',
        'token_key': 'token-key',
        'token_secret': 'token-secret',
        'token_name': 'token-name',
        'consumer_key': 'consumer-key',
        'consumer_secret': 'consumer-secret',
        'date_created': '2013-01-11 12:43:23',
        'date_updated': '2013-01-11 12:43:23'
    }
    TWOFACTOR_REQUIRED_RESPONSE = {
        'code': 'TWOFACTOR_REQUIRED',
        'message': 'This account requires 2-factor authentication.',
        'extra': {}
    }
    TWOFACTOR_ERR_RESPONSE = {
        'message': 'The provided 2-factor key is not recognised.',
        'code': 'TWOFACTOR_FAILURE',
        'extra': {}
    }
    LOGIN_ERR_RESPONSE = {
        'code': 'INVALID_CREDENTIALS',
        'message': "Your email/password isn't correct.",
        'extra': {}
    }
    REGISTER_CREATED_RESPONSE = {
        'status': 'Active',
        'openid': 'fFcTeSt',
        'verified': False,
        'emails': [
            {
                'href': '/api/v2/emails/new@te.st',
                'verified': False
            }
        ],
        'tokens': [],
        'href': '/api/v2/accounts/fFcTeSt',
        'displayname': 'Test',
        'email': 'new@te.st'
    }

    def do_GET(self):
        """Handle GET as if we were the one.ubuntu.com server."""
        parsed_path = urlparse.urlparse(self.path)
        query_dict = urlparse.parse_qs(parsed_path.query)

        if parsed_path.path.startswith('/oauth/sso-finished-so-get-tokens/'):
            self._handle_get_tokens(query_dict)
        else:
            self.send_error(404)  # real server currently returns 500!

    def _handle_get_tokens(self, query_dict):
        for k in ['platform', 'platform_version', 'platform_arch']:
            if k not in query_dict:
                logger.debug(
                    '****** MISSING %s IN %s. Bailing.', k, query_dict)
                self.send_error(401)
                break
        else:
            self.send_response(200)
            self.end_headers()
            self.wfile.write("ok 0/0")  # fake format of server's real response

    def do_POST(self):
        """Handle POST as if we are the login.ubuntu.com server."""
        length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(length)
        if post_data == '':
            logger.debug('NOTE: empty post data, just dropping the request.')
        else:
            self._handle_post(post_data)

    def _handle_post(self, post_data):
        try:
            body_dict = json.loads(post_data)
        except ValueError:
            logger.debug('Error loading json from post data: "%s"', post_data)
            self.send_error(500)
        else:
            self._handle_post_with_json_data(body_dict)

    def _handle_post_with_json_data(self, body_dict):
        if self.path == self.SSO_API_PATH + '/tokens/oauth':
            self._handle_login(body_dict)
        elif self.path == self.SSO_API_PATH + '/accounts':
            self._handle_register(body_dict)
        else:
            logger.debug('unknown -- self.path is %s', self.path)
            self.send_error(404, 'unknown')

    def send_json_reply(self, code, reply_json):
        self.send_response(code)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(reply_json)

    def _handle_login(self, body_dict):
        if ('email' not in body_dict or 'password' not in body_dict or
                'token_name' not in body_dict):
            self.send_json_reply(400, json.dumps(self.INVALID_DATA_RESPONSE))
        else:
            self._handle_login_with_valid_data(body_dict)

    def _handle_login_with_valid_data(self, body_dict):
        email = body_dict['email']

        if email in ['ok@te.st', 'new@te.st']:
            self.send_json_reply(201, json.dumps(self.LOGIN_OK_RESPONSE))
        elif email == '2fa@te.st':
            self._handle_2fa_login(body_dict)
        else:
            self.send_json_reply(401, json.dumps(self.LOGIN_ERR_RESPONSE))

    def _handle_2fa_login(self, body_dict):
        if 'otp' not in body_dict:
            self.send_json_reply(
                401, json.dumps(self.TWOFACTOR_REQUIRED_RESPONSE))
        elif len(body_dict['otp']) != 6:
            self.send_json_reply(403, json.dumps(self.TWOFACTOR_ERR_RESPONSE))
        else:
            self.send_json_reply(201, json.dumps(self.LOGIN_OK_RESPONSE))

    def _handle_register(self, body_dict):
        if ('email' not in body_dict or
                'password' not in body_dict or
                'displayname' not in body_dict or
                body_dict['email'] == 'bad-new@te.st'):

            self.send_json_reply(400, json.dumps(self.INVALID_DATA_RESPONSE))
        else:
            self.send_json_reply(
                201, json.dumps(self.REGISTER_CREATED_RESPONSE))
