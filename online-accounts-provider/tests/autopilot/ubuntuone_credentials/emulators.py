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

from autopilot import input

from ubuntuuitoolkit import emulators as toolkit_emulators


class NewAccount(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """New Account Autopilot emulator."""

    def is_error_label_visible(self):
        error_label = self.select_single(objectName='errorLabel')
        return error_label.visible

    def log_in(self, email, password):
        self._fill_log_in(email, password)
        self._click_continue()

    def enter_twofactor_code(self, twoFactorCode):
        two_factor_text_field = self.select_single(
            TextField, objectName='twoFactorTextField')
        two_factor_text_field.write(twoFactorCode)
        self._click_continue()

    def _fill_log_in(self, email, password):
        self._enter_email(email)
        self._enter_login_password(password)

    def _enter_email(self, email):
        email_text_field = self.select_single(
            TextField, objectName='emailTextField')
        email_text_field.write(email)

    def _enter_login_password(self, password):
        password_text_field = self.select_single(
            TextField, objectName='loginFormPasswordTextField')
        password_text_field.write(password)

    def _click_continue(self):
        continue_button = self.select_single(
            objectName='continueButton', visible=True)
        self.pointing_device.click_object(continue_button)

    def register_new_account(self, email, name, password,
                             password_confirmation, agree_to_terms):
        self._switch_to_new_account()
        self._enter_email(email)
        self._enter_name(name)
        self._enter_password_new_user(password)
        self._enter_password_confirmation(password_confirmation)
        self._agree_to_terms(agree_to_terms)
        self._click_continue()

    def _switch_to_new_account(self):
        new_user_switch = self.select_single(
            toolkit_emulators.CheckBox, objectName='newUserToggleSwitch')
        new_user_switch.check()

    def _enter_name(self, name):
        name_text_field = self.select_single(
            TextField, objectName='nameTextField')
        name_text_field.write(name)

    def _enter_password_new_user(self, password):
        password_text_field = self.select_single(
            TextField, objectName='passwordTextField')
        password_text_field.write(password)

    def _enter_password_confirmation(self, password_confirmation):
        confirm_password_text_field = self.select_single(
            TextField, objectName='confirmPasswordTextField')
        confirm_password_text_field.write(password_confirmation)

    def _agree_to_terms(self, agree):
        terms_and_conditions_check_box = self.select_single(
            toolkit_emulators.CheckBox,
            objectName='termsAndConditionsCheckBox')
        if agree:
            terms_and_conditions_check_box.check()
        else:
            terms_and_conditions_check_box.uncheck()


class TextField(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """TextField Autopilot emulator."""
    # TODO change this to use the text field emulator, once it lands on
    # ubuntu-ui-toolkit. --elopio - 2013-10-08

    def __init__(self, *args):
        super(TextField, self).__init__(*args)
        self.keyboard = input.Keyboard.create()

    def write(self, text):
        """Write into the text field.

        :parameter text: The text to write.

        """
        self.pointing_device.click_object(self)
        self.keyboard.type(text)
