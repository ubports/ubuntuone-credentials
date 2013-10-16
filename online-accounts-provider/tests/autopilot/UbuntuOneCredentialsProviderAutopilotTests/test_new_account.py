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
from ubuntuuitoolkit import emulators as toolkit_emulators

import UbuntuOneCredentialsProviderAutopilotTests
from UbuntuOneCredentialsProviderAutopilotTests import emulators


_VALID_NEW_USER = dict(
    email='valid@example.com', name='Name', password='password',
    password_confirmation='password', agree_to_terms=True)


class NewUbuntuOneOnlineAccountTestCase(
        UbuntuOneCredentialsProviderAutopilotTests.TestCaseWithQMLWrapper):

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    def setUp(self):
        super(NewUbuntuOneOnlineAccountTestCase, self).setUp()
        self.new_account = self.main_view.select_single(emulators.NewAccount)

    def test_loading_overlay_starts_invisible(self):
        overlay = self.main_view.select_single(objectName='loadingOverlay')
        self.assertThat(overlay.visible, Eventually(Equals(False)))

    def test_email_field_visible_focused(self):
        email_text_field = self.main_view.select_single(
            objectName='emailTextField')
        self.assertThat(email_text_field.visible, Eventually(Equals(True)))
        self.assertThat(email_text_field.focus, Eventually(Equals(True)))

    def test_login_initial_state(self):
        password_text_field = self.main_view.select_single(
            objectName='loginFormPasswordTextField')
        self.assertThat(password_text_field.visible, Eventually(Equals(True)))
        two_factor_text_field = self.main_view.select_single(
            objectName='twoFactorTextField')
        self.assertThat(
            two_factor_text_field.visible, Eventually(Equals(False)))

    def test_switch_to_new_user(self):
        new_user_switch = self.main_view.select_single(
            toolkit_emulators.CheckBox, objectName='newUserToggleSwitch')
        new_user_switch.check()

        password_text_field = self.main_view.select_single(
            objectName='loginFormPasswordTextField')
        self.assertThat(password_text_field.visible, Eventually(Equals(False)))

        name_text_field = self.main_view.select_single(
            objectName='nameTextField')
        self.assertThat(name_text_field.visible, Eventually(Equals(True)))

    def test_error_label_starts_invisible(self):
        self.assertThat(
            self.new_account.is_error_label_visible(),
            Eventually(Equals(False)))


class LogInUbuntuOneOnlineAccountErrorsTestCase(
        UbuntuOneCredentialsProviderAutopilotTests.TestCaseWithQMLWrapper):

    scenarios = [
        ('no input', dict(email='', password='')),
        ('invalid email', dict(email='invalid', password='password'))
    ]

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    def test_log_in_error(self):
        new_account = self.main_view.select_single(emulators.NewAccount)
        new_account.log_in(self.email, self.password)

        self.assertThat(
            new_account.is_error_label_visible(),
            Eventually(Equals(True)))


class NewUbuntuOneOnlineAccountErrorsTestCase(
        UbuntuOneCredentialsProviderAutopilotTests.TestCaseWithQMLWrapper):

    scenarios = [
        ('no name', dict(_VALID_NEW_USER, name='')),
        ('no password', dict(_VALID_NEW_USER, password='')),
        ('password mismatch', dict(
            _VALID_NEW_USER, password_confirmation='different password')),
        ('short password', dict(
            _VALID_NEW_USER, password='short', password_confirmation='short'))
    ]

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    def test_new_user_error(self):
        new_account = self.main_view.select_single(emulators.NewAccount)
        new_account.register_new_account(
            self.email, self.name, self.password, self.password_confirmation,
            self.agree_to_terms)

        self.assertThat(
            new_account.is_error_label_visible(), Eventually(Equals(True)))