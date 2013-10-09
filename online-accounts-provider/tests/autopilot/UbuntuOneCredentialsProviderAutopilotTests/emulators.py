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


class TextField(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """TextField Autopilot emulator."""
    # TODO change this to use the text field emulator, once it lands on
    # ubuntu-ui-toolkit. --elopio - 2013-10-08

    def __init__(self, *args):
        super(TextField, self).__init__(*args)
        self.keyboard = input.Keyboard.create()

    def write(self, text, clear=True):
        """Write into the text field.

        :parameter text: The text to write.

        """
        self.pointing_device.click_object(self)
        self.keyboard.type(text)
