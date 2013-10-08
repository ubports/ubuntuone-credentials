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

from autopilot.matchers import Eventually
from testtools.matchers import Equals

import UbuntuOneCredentialsProviderAutopilotTests


class ExistingUOAAccountTests(
        UbuntuOneCredentialsProviderAutopilotTests.TestCaseWithQMLWrapper):

    test_qml_wrapper_file_name = 'TestWrapperExisting.qml'

    def test_show_remove_account_dialog(self):
        remove_account_button = self.main_view.select_single(
            objectName='removeAccountButton')
        self.pointing_device.click_object(remove_account_button)

        confirm_button = self.main_view.select_single(
            objectName='confirmRemoveAccountButton')

        self.assertThat(
            confirm_button.visible, Eventually(Equals(True)))

        cancel_button = self.main_view.select_single(
            objectName='cancelRemoveAccountButton')
        self.pointing_device.click_object(cancel_button)

    def test_shows_expected_email(self):
        email_list_item = self.main_view.select_single(
            objectName='emailListItem')
        self.assertThat(
            email_list_item.value, Eventually(Equals("my@email.com")))
