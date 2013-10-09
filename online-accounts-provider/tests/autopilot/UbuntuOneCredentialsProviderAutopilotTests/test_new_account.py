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

    def test_login_error_on_no_input(self):
        self.new_account.log_in(email='', password='')

        self.assertThat(
            self.new_account.is_error_label_visible(),
            Eventually(Equals(True)))
        
    def _click_continue_button_from(self, parent_object_name):
        parent = self.main_view.select_single(objectName=parent_object_name)
        continue_button = parent.select_single(objectName='continueButton')
        self.pointing_device.click_object(continue_button)

    def test_login_error_on_bad_email(self):
        self.new_account.log_in(
            email='invalid email', password='password')

        self.assertThat(
            self.new_account.is_error_label_visible(),
            Eventually(Equals(True)))

    def test_new_user_error_on_no_name(self):
        self._setup_new_user_errors()
        self._click_continue_button_from("buttonRow-RegisterForm")
        self.assertThat(
            self.error_label.visible, Eventually(Equals(True)))

    def _setup_new_user_errors(self):
        self.email_text_field = self.main_view.select_single(
            objectName='emailTextField')
        self.pointing_device.click_object(self.email_text_field)
        self.keyboard.type('valid@email.com')
        self.assertThat(
            self.email_text_field.acceptableInput, Eventually(Equals(True)))
        self.error_label = self.main_view.select_single(
            objectName='errorLabel')
        new_user_switch = self.main_view.select_single(
            emulators.CheckBox, objectName='newUserToggleSwitch')
        new_user_switch.check()

    def test_new_user_error_on_no_password(self):
        self._setup_new_user_errors()
        name_text_field = self.main_view.select_single(
            objectName='nameTextField')
        self.pointing_device.click_object(name_text_field)
        self.keyboard.type('A Name')
        self._click_continue_button_from('buttonRow-RegisterForm')
        self.assertThat(
            self.error_label.visible, Eventually(Equals(True)))

    def test_new_user_error_on_no_password_match(self):
        self._setup_new_user_errors()
        name_text_field = self.main_view.select_single(
            objectName='nameTextField')
        self.pointing_device.click_object(name_text_field)
        self.keyboard.type('A Name')

        password_text_field = self.main_view.select_single(
            objectName='passwordTextField')
        self.pointing_device.click_object(password_text_field)
        self.keyboard.type('12345678910111213141516')

        confirm_password_text_field = self.main_view.select_single(
            objectName='confirmPasswordTextField')
        self.pointing_device.click_object(confirm_password_text_field)
        self.keyboard.type('123456789ABCDEF')

        terms_and_conditions_check_box = self.main_view.select_single(
            emulators.CheckBox, objectName='termsAndConditionsCheckBox')
        terms_and_conditions_check_box.check()
        self._click_continue_button_from('buttonRow-RegisterForm')
        self.assertThat(
            self.error_label.visible, Eventually(Equals(True)))

    def test_new_user_error_on_short_password(self):
        self._setup_new_user_errors()

        name_text_field = self.main_view.select_single(
            objectName='nameTextField')
        self.pointing_device.click_object(name_text_field)
        self.keyboard.type('A Name')

        password_text_field = self.main_view.select_single(
            objectName='passwordTextField')
        self.pointing_device.click_object(password_text_field)
        self.keyboard.type('123')

        confirm_password_text_field = self.main_view.select_single(
            objectName='confirmPasswordTextField')
        self.pointing_device.click_object(confirm_password_text_field)
        self.keyboard.type('123')

        terms_and_conditions_check_box = self.main_view.select_single(
            emulators.CheckBox, objectName='termsAndConditionsCheckBox')
        terms_and_conditions_check_box.check()
        self._click_continue_button_from('buttonRow-RegisterForm')
        self.assertThat(
            self.error_label.visible, Eventually(Equals(True)))
