# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013-2014 Canonical Ltd.
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

import os

from autopilot.matchers import Eventually
from testtools.matchers import Equals, Not
from ubuntuuitoolkit import emulators as toolkit_emulators

from ubuntuone_credentials import (
    emulators,
    TestCaseWithQMLWrapper
)


class NewUbuntuOneOnlineAccountTestCase(TestCaseWithQMLWrapper):

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


class SimpleLogInTestCase(TestCaseWithQMLWrapper):

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    scenarios = [
        ('success',
         dict(email='ok@te.st', password='password', success=True)),
        ('failure',
         dict(email='not-ok@te.st', password='password', success=False))]

    def setUp(self):
        # Start the fake server before launching the application.
        if (os.environ.get('SSO_AUTH_BASE_URL') == 'fake' and
                os.environ.get('SSO_UONE_BASE_URL') == 'fake'):
            self.use_fake_servers()
        super(SimpleLogInTestCase, self).setUp()

    def test_simple_login(self):
        """Test that success pops the NewAccount main page, and
        failure shows the error label.
        """
        new_account = self.main_view.select_single(emulators.NewAccount)
        new_account.log_in(email=self.email, password=self.password)
        dummyPage = self.main_view.select_single(objectName="dummyPage")
        self.assertThat(dummyPage.visible, Eventually(Equals(self.success)))
        self.assertThat(new_account.is_error_label_visible(),
                        Eventually(Equals(not self.success)))


class TwoFactorLogInTestCase(TestCaseWithQMLWrapper):

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    scenarios = [
        ('success',
         dict(email='2fa@te.st', password='password',
              twoFactorCode='123456', success=True)),
        ('failure',
         dict(email='2fa@te.st', password='password',
              twoFactorCode='bad', success=False))
        ]

    def setUp(self):
        # Start the fake server before launching the application.
        if (os.environ.get('SSO_AUTH_BASE_URL') == 'fake' and
                os.environ.get('SSO_UONE_BASE_URL') == 'fake'):
            self.use_fake_servers()
        super(TwoFactorLogInTestCase, self).setUp()

    def test_twofactor_login(self):
        "Test that success pops the NewAccount main page."

        # Layout change workaround: Here we have to save the initial
        # globalRect of the twoFactorTextField because we know it will
        # move, but not necessarily before we are notified that the
        # visible flag is set - so we need to be able to wait for it
        # to move so that the call to enter_twofactor_code() below
        # clicks on the right text field. If we don't do this,
        # depending on the timing of those update signals,
        # enter_twofactor_code() may end up writing the code into the
        # password field, which is always at the location that the
        # hidden two-factor field starts out at.
        tfn = 'twoFactorTextField'
        two_factor_field = self.main_view.select_single(emulators.TextField,
                                                        objectName=tfn)
        saved_rect = two_factor_field.globalRect

        new_account = self.main_view.select_single(emulators.NewAccount)
        new_account.log_in(email=self.email, password=self.password)

        # Here we wait for both visible and focus, to ensure that the
        # field is ready for input, then we also wait for the rect to
        # change, so we know that autopilot will have the right
        # coordinates to click on it.
        self.assertThat(two_factor_field.visible, Eventually(Equals(True)))
        self.assertThat(two_factor_field.focus, Eventually(Equals(True)))
        self.assertThat(two_factor_field.globalRect,
                        Eventually(Not(Equals(saved_rect))))

        new_account.enter_twofactor_code(self.twoFactorCode)

        dummyPage = self.main_view.select_single(objectName="dummyPage")
        self.assertThat(dummyPage.visible, Eventually(Equals(self.success)))
        self.assertThat(new_account.is_error_label_visible(),
                        Eventually(Equals(not self.success)))


VALID_NEW_ACCOUNT = dict(email='new@te.st', name='test name',
                         password='password',
                         password_confirmation='password',
                         agree_to_terms=True,
                         success=True)


class RegisterNewU1AccountTestCase(TestCaseWithQMLWrapper):

    test_qml_wrapper_file_name = 'TestWrapperNew.qml'

    scenarios = [
        ('success', VALID_NEW_ACCOUNT),
        ('bad request', dict(VALID_NEW_ACCOUNT, email='bad-new@te.st',
                             success=False))
        ]

    def setUp(self):
        # Start the fake server before launching the application.
        if (os.environ.get('SSO_AUTH_BASE_URL') == 'fake' and
                os.environ.get('SSO_UONE_BASE_URL') == 'fake'):
            self.use_fake_servers()
        super(RegisterNewU1AccountTestCase, self).setUp()

    def test_register_new_account(self):
        "Test that registering a new account pops the main page."

        new_account = self.main_view.select_single(emulators.NewAccount)
        new_account.register_new_account(self.email, self.name, self.password,
                                         self.password_confirmation,
                                         self.agree_to_terms)

        dummyPage = self.main_view.select_single(objectName="dummyPage")
        self.assertThat(dummyPage.visible, Eventually(Equals(self.success)))
        self.assertThat(new_account.is_error_label_visible(),
                        Eventually(Equals(not self.success)))
