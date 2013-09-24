# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-

"""Tests for launching the plugin"""

from autopilot.matchers import Eventually
from testtools.matchers import Equals
import os
from UbuntuOneCredentialsProviderAutopilotTests import UbuntuTouchAppTestCase


class NewUOAAccountTests(UbuntuTouchAppTestCase):

    test_qml_path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                 "../../")
    test_qml_filename = "TestWrapperNew.qml"
    test_qml_file = os.path.join(test_qml_path, test_qml_filename)

    def setUp(self):
        super(NewUOAAccountTests, self).setUp()
        self.uoa_account = self.get_object(objectName="FakeNewUOAAccount")

    def test_select_mainView(self):
        mainView = self.get_mainview()
        self.assertThat(mainView.visible, Eventually(Equals(True)))

    def test_loadingOverlay_starts_invisible(self):
        overlay = self.get_object(objectName="loadingOverlay")
        self.assertThat(overlay.visible, Eventually(Equals(False)))

    def test_email_field_visible_focused(self):
        emailTextField = self.get_object(objectName="emailTextField")
        self.assertThat(emailTextField.visible, Eventually(Equals(True)))
        self.assertThat(emailTextField.focus, Eventually(Equals(True)))

    def test_login_initial_state(self):
        passwordTextField = self.get_object("loginFormPasswordTextField")
        self.assertThat(passwordTextField.visible, Eventually(Equals(True)))
        twoFactorTextField = self.get_object(objectName="twoFactorTextField")
        self.assertThat(twoFactorTextField.visible, Eventually(Equals(False)))

    def test_switch_to_newuser(self):

        self.mouse_click(objectName="newUserToggleSwitch")

        passwordField = self.get_object("loginFormPasswordTextField")
        self.assertThat(passwordField.visible, Eventually(Equals(False)))

        nameTextField = self.get_object("nameTextField")
        self.assertThat(nameTextField.visible, Eventually(Equals(True)))

    def click_continuebutton_from(self, parentObjectName):
        parent = self.get_object(parentObjectName)
        continueButton = parent.select_single(objectName="continueButton")
        self.pointing_device.move_to_object(continueButton)
        self.pointing_device.click()

    def test_login_error_on_no_input(self):
        errorTextField = self.get_object("errorLabel")
        self.assertThat(errorTextField.visible, Eventually(Equals(False)))
        self.click_continuebutton_from("buttonRow-LoginForm-noTwoFactor")
        self.assertThat(errorTextField.visible, Eventually(Equals(True)))

    def test_login_error_on_bad_email(self):
        errorTextField = self.get_object("errorLabel")
        self.assertThat(errorTextField.visible, Eventually(Equals(False)))
        self.mouse_click("emailTextField")
        self.type_string("this isn't a valid email")

        self.click_continuebutton_from("buttonRow-LoginForm-noTwoFactor")
        self.assertThat(errorTextField.visible, Eventually(Equals(True)))

    def _setup_newuser_errors(self):
        self.emailTextField = self.get_object("emailTextField")
        self.mouse_click("emailTextField")
        self.type_string("valid@email.com")
        self.assertThat(self.emailTextField.acceptableInput,
                        Eventually(Equals(True)))
        self.errorTextField = self.get_object("errorLabel")
        self.mouse_click(objectName="newUserToggleSwitch")

    def test_newuser_error_on_no_name(self):
        self._setup_newuser_errors()
        self.click_continuebutton_from("buttonRow-RegisterForm")
        self.assertThat(self.errorTextField.visible, Eventually(Equals(True)))

    def test_newuser_error_on_no_password(self):
        self._setup_newuser_errors()
        self.mouse_click("nameTextField")
        self.type_string("A Name")
        self.click_continuebutton_from("buttonRow-RegisterForm")
        self.assertThat(self.errorTextField.visible, Eventually(Equals(True)))

    def test_newuser_error_on_no_password_match(self):
        self._setup_newuser_errors()
        self.mouse_click("nameTextField")
        self.type_string("A Name")

        self.mouse_click("passwordTextField")
        self.type_string("12345678910111213141516")
        self.mouse_click("confirmPasswordTextField")
        self.type_string("123456789ABCDEF")
        self.mouse_click(objectName="termsAndConditionsCheckBox")
        self.click_continuebutton_from("buttonRow-RegisterForm")
        self.assertThat(self.errorTextField.visible, Eventually(Equals(True)))

    def test_newuser_error_on_short_password(self):
        self._setup_newuser_errors()
        self.mouse_click("nameTextField")
        self.type_string("A Name")
        self.mouse_click("passwordTextField")
        self.type_string("123")
        self.mouse_click("confirmPasswordTextField")
        self.type_string("123")
        self.mouse_click(objectName="termsAndConditionsCheckBox")
        self.click_continuebutton_from("buttonRow-RegisterForm")
        self.assertThat(self.errorTextField.visible, Eventually(Equals(True)))


class ExistingUOAAccountTests(UbuntuTouchAppTestCase):

    test_qml_path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                 "../../")
    test_qml_filename = "TestWrapperExisting.qml"
    test_qml_file = os.path.join(test_qml_path, test_qml_filename)

    def setUp(self):
        super(ExistingUOAAccountTests, self).setUp()
        self.uoa_account = self.get_object(objectName="FakeExistingUOAAccount")

    def test_show_remove_account_dialog(self):

        self.mouse_click(objectName="removeAccountButton")

        confirmRemoveButton = self.get_object("confirmRemoveAccountButton")

        self.assertThat(confirmRemoveButton.visible,
                        Eventually(Equals(True)))

        self.mouse_click(objectName="cancelRemoveAccountButton")

    def test_shows_expected_email(self):
        emailListItem = self.get_object("emailListItem")
        self.assertThat(emailListItem.value,
                        Eventually(Equals("my@email.com")))
