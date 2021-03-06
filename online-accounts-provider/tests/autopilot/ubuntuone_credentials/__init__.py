# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2015 Canonical Ltd.
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

import ubuntuuitoolkit
from autopilot import platform
from ubuntu_keyboard.emulators import keyboard


class NewAccount(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    """New Account Autopilot helper."""

    def is_error_label_visible(self):
        error_label = self.select_single(objectName='errorLabel')
        return error_label.visible

    def log_in(self, email, password):
        self._fill_log_in(email, password)
        self._click_continue()

    def enter_twofactor_code(self, twoFactorCode):
        two_factor_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='twoFactorTextField')
        two_factor_text_field.write(twoFactorCode)
        self._click_continue()

    def _fill_log_in(self, email, password):
        self._enter_email(email)
        self._enter_login_password(password)

    def _enter_email(self, email):
        email_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='emailTextField')
        email_text_field.write(email)

    def _enter_login_password(self, password):
        password_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='loginFormPasswordTextField')
        password_text_field.write(password)

    def _click_continue(self):
        continue_button = self.select_single(
            objectName='continueButton', visible=True)
        self.pointing_device.click_object(continue_button)

    def register_new_account(self, email, name, password,
                             password_confirmation, agree_to_terms):
        self.switch_to_new_account()
        self._enter_email(email)
        self._enter_name(name)
        self._enter_password_new_user(password)
        self._enter_password_confirmation(password_confirmation)
        self._agree_to_terms(agree_to_terms)
        self._click_continue()

    def switch_to_new_account(self):
        new_user_label = self.select_single(
            'Label', objectName='loginFormToggleLabel')
        self.pointing_device.click_object(new_user_label)

    def _enter_name(self, name):
        name_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='nameTextField')
        name_text_field.write(name)

    def _enter_password_new_user(self, password):
        self._dismiss_on_screen_keyboard()
        password_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='newPasswordTextField')
        password_text_field.write(password)

    def _dismiss_on_screen_keyboard(self):
        # XXX instead of dismissing the keyboard, the toolkit should take it
        # into account when swiping things into view. See bug #1442733.
        # -- elopio - 2015-02-24
        if platform.model() != 'Desktop':
            keyboard.Keyboard().dismiss()

    def _enter_password_confirmation(self, password_confirmation):
        self._dismiss_on_screen_keyboard()
        confirm_password_text_field = self.select_single(
            ubuntuuitoolkit.TextField, objectName='confirmPasswordTextField')
        confirm_password_text_field.write(password_confirmation)

    def _agree_to_terms(self, agree):
        self._dismiss_on_screen_keyboard()
        terms_and_conditions_check_box = self.select_single(
            ubuntuuitoolkit.CheckBox,
            objectName='termsAndConditionsCheckBox')
        if agree:
            terms_and_conditions_check_box.check()
        else:
            terms_and_conditions_check_box.uncheck()
