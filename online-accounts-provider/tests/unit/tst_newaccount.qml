/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import "utils.js" as Utils
import "../../"

Item {
    // The objects
    NewAccount {
        id: newAccount
        anchors.fill: parent
    }

    Rectangle {
        /* fake to isolate NewAccount from Main */
        id: loadingOverlay
    }
        
    
    TestCase {
        name: "NewAccountTests"
        id: testCase

        property var loginForm
        property var registerForm
        property var errorLabel
        property var emailTextField
        
        function init() {
            newAccount.resetUI();
        }
        
        function cleanup() {
        }
        
        function initTestCase() {
            loginForm = Utils.findChild(newAccount, "loginForm")
            registerForm = Utils.findChild(newAccount, "registerForm")
            errorLabel = Utils.findChild(newAccount, "errorLabel")
            emailTextField = Utils.findChild(newAccount, "emailTextField")
        }
        
        function cleanupTestCase() {
        }
        
        function test_resetUI() {

            compare(newAccount.currentVisible, loginForm);
            compare(loginForm.visible, true);
            compare(errorLabel.visible, false);
            compare(emailTextField.text, "");
            compare(newAccount.state, "login");

            var loginForm_pwd = Utils.findChild(loginForm, "loginFormPasswordTextField");
            compare(loginForm_pwd.text, "");

            compare(loginForm.twoFactorVisible, false);
        }

        function test_toggleNewUser() {

            compare(loginForm.visible, true);
            compare(registerForm.visible, false);
            compare(newAccount.state, "login");

            newAccount.toggleNewUser();

            compare(loginForm.visible, false);
            compare(registerForm.visible, true);
            compare(newAccount.state, "register");

        }

        function test_showTwoFactorUI() {
            compare(loginForm.twoFactorVisible, false);
            compare(newAccount.state, "login");
            newAccount.showTwoFactorUI();
            compare(newAccount.state, "twofactor");
            compare(loginForm.twoFactorVisible, true);
        }

        function test_login_validity_data() {
            var invalidEmail = "invalid email";
            var validEmail = "valid@email.com";
            var validPassword = "123456789";
            var validTwoFactorCode = "123456";
            return [
                { shouldShowError: true, tag: "empty input no 2fa", showTwoFactor: false, email: "", password: "", twoFactorCode: ""},
                { shouldShowError: true, tag: "empty input, with 2fa visible", showTwoFactor: true, email: "", password: "", twoFactorCode: ""},
                { shouldShowError: true, tag: "invalid email", showTwoFactor: false, email: invalidEmail, password: validPassword, twoFactorCode: ""},      
                { shouldShowError: true, tag: "invalid email with 2fa", showTwoFactor: true, email: invalidEmail, password: validPassword, twoFactorCode: validTwoFactorCode},
                { shouldShowError: true, tag: "short pwd", showTwoFactor: false, email: validEmail, password: "123", twoFactorCode: ""},
                { shouldShowError: true, tag: "short pwd with 2fa", showTwoFactor: true, email: validEmail, password: "123", twoFactorCode: validTwoFactorCode},

                { shouldShowError: false, tag: "OK no 2fa", showTwoFactor: false, email: validEmail, password: validPassword, twoFactorCode: ""},
                { shouldShowError: false, tag: "OK with 2fa", showTwoFactor: true, email: validEmail, password: validPassword, twoFactorCode: validTwoFactorCode},
            ]
        }

        function test_login_validity(data) {
            newAccount.resetUI();
            emailTextField.text = data.email;
            var passwordTextField = Utils.findChild(loginForm, "loginFormPasswordTextField");            
            passwordTextField.text = data.password;
            if(data.showTwoFactor) {
                newAccount.showTwoFactorUI();
                var twoFactorTextField = Utils.findChild(loginForm, "twoFactorTextField");
                twoFactorTextField.text = data.twoFactorCode;
            }
            
            newAccount.validateInput();
            compare(errorLabel.visible, data.shouldShowError);
        }

        function test_register_validity_data() {
            var invalidEmail = "invalid email";
            var validEmail = "valid@email.com";
            var invalidName = "";
            var validName = "Vlad Name";
            var validPassword = "123456789";
            return [
                { shouldShowError: true, tag: "invalid email", email: "invalid", name: validName, password: validPassword, confirmPassword: validPassword, termsChecked: true},
                { shouldShowError: true, tag: "empty name", email: validEmail, name: "", password: validPassword, confirmPassword: validPassword, termsChecked: true},
                { shouldShowError: true, tag: "short pwd", email: validEmail, name: validName, password: "123", confirmPassword: "123", termsChecked: true},
                { shouldShowError: true, tag: "pwd mismatch", email: validEmail, name: validName, password: validPassword, confirmPassword: "123456789-mismatch", termsChecked: true},
                { shouldShowError: true, tag: "termsnotchecked", email: validEmail, name: validName, password: validPassword, confirmPassword: "123456789-mismatch", termsChecked: false},
                { shouldShowError: false, tag: "everything OK", email: validEmail, name: validName, password: validPassword, confirmPassword: validPassword, termsChecked: true},
            ]
        }

        function test_register_validity(data) {
            newAccount.resetUI();
            newAccount.toggleNewUser();
            emailTextField.text = data.email;
            var nameTextField = Utils.findChild(registerForm, "nameTextField");
            nameTextField.text = data.name;
            var passwordTextField = Utils.findChild(registerForm, "passwordTextField");
            passwordTextField.text = data.password;
            var registerForm_confirm_pwd = Utils.findChild(registerForm, "confirmPasswordTextField");
            registerForm_confirm_pwd.text = data.confirmPassword;
            var termsCheckbox = Utils.findChild(registerForm, "termsAndConditionsCheckBox");
            termsCheckbox.checked = data.termsChecked;

            newAccount.validateInput()

            compare(errorLabel.visible, data.shouldShowError);   
        }
    }
}
