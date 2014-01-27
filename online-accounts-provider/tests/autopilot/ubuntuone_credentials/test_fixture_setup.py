# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

import httplib
import urlparse

import testtools

from ubuntuone_credentials import fixture_setup


class FakeSSOAndU1ServerRunningTestCase(testtools.TestCase):

    def setUp(self):
        super(FakeSSOAndU1ServerRunningTestCase, self).setUp()
        fake_search_server = fixture_setup.FakeSSOAndU1ServersRunning()
        self.addCleanup(self.assert_server_not_running)
        self.useFixture(fake_search_server)

        self.netloc = urlparse.urlparse(fake_search_server.url).netloc

    def assert_server_not_running(self):
        connection = httplib.HTTPConnection(self.netloc)
        self.assertRaises(Exception, self.do_get_request, connection)

    def do_get_request(self, connection):
        connection.request('GET', '/oauth/sso-finished-so-get-tokens/')

    def do_post_request(self, connection):
        connection.request('POST', '/api/v2/tokens/oauth', 'dummy body')

    def test_fake_sso_and_u1_server_must_accept_get_request(self):
        connection = httplib.HTTPConnection(self.netloc)
        self.addCleanup(connection.close)

        self.do_get_request(connection)
        self.assertEqual(connection.getresponse().status, 401)

    def test_fake_sso_and_u1_server_must_accept_post_request(self):
        connection = httplib.HTTPConnection(self.netloc)
        self.addCleanup(connection.close)

        self.do_post_request(connection)
        self.assertEqual(connection.getresponse().status, 500)
