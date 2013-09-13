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
import Ubuntu.Components 0.1
import UbuntuOne 1.0
import Ubuntu.OnlineAccounts 0.1


Column {
    id: main

    property bool isNewAccount: false
    property variant __account: account

    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm
    property var formValid: false

    signal finished

    anchors.margins: units.gu(1)

    spacing: units.gu(2)

    Component.onCompleted: {
        isNewAccount = (__account.accountId === 0);
        enableAccount();
        __account.sync()
        resetUI();
    }

    Label {
        id: title
        text: "One account to log in to everything on Ubuntu"
        color: UbuntuColors.coolGrey
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    Label {
        id: errorLabel
        text: ""
        font.bold: true
        color: "red"
        visible: false

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    Label {
        text: "Please type your email:"

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    TextField {
        id: emailTextField
        placeholderText: "Your email"
        width: main.width - (2 * main.anchors.margins)
        anchors.left: parent.left
        anchors.margins: parent.anchors.margins
        validator: RegExpValidator { regExp: /.+@.+/ }
        focus: true
        KeyNavigation.tab: loginForm.visible ? loginForm.passwordTextField : registerForm.nameTextField
        KeyNavigation.backtab: loginForm.visible ? ( loginForm.twoFactorVisible ? loginForm.twoFactorTextField : loginForm.passwordTextField) : registerForm.confirmPasswordTextField
    }

    Row {
        spacing: units.gu(2)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins

        Switch {
            id: newUserToggleSwitch
            checked: false

            onCheckedChanged: {
                toggleNewUser();
            }
        }

        Label {
            anchors.verticalCenter: newUserToggleSwitch.verticalCenter
            text: "I am a new Ubuntu One user"
        }

    } // Row

    LoginForm {
        id: loginForm
        visible: true
        
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    RegisterForm {
        id: registerForm
        visible: false
        
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    // -------------------------------------------------

    UbuntuOneCredentialsService {
        id: u1credservice

        onLoginOrRegisterSuccess: {
            handleSuccess();
        }

        onTwoFactorAuthRequired: {
            showTwoFactorUI();
        }

        onLoginOrRegisterError: {
            if (errorMessage == "Invalid request data") {
                errorMessage = "Please enter a valid email address.";
            }
            showError(errorMessage);
        }
    }

    function resetUI() {
        errorLabel.visible = false;
        emailTextField.text = "";
        loginForm.resetUI()
        registerForm.resetUI();
        newUserToggleSwitch.checked = false;
        state = "login";
        switchTo(loginForm);
        formValid = false;
        emailTextField.forceActiveFocus();
    }

    function showError(message) {
        errorLabel.text = message;
        errorLabel.visible = true;
        loadingOverlay.visible = false;
    }

    function switchTo(newVisible) {
        currentVisible.visible = false;
        newVisible.visible = true;
        currentVisible = newVisible;
    }

    function toggleNewUser() {
        if(state == "login") {
            switchTo(registerForm)
            state = "register";
            registerForm.nameTextField.focus = true;
        } else if(state == "register") {
            switchTo(loginForm)
            state = "login";
            loginForm.passwordTextField.focus = true;
        } else {
            console.debug("unexpected state" + state + "in toggleNewUser");
        }
    }

    /* processForm uses a timer to delay calling u1credservice, which can
       take a while, to ensure that the loadingOverlay is shown
       immediately.
       */
    function processForm() {
        loadingOverlay.visible = true;
        formSubmitTimer.running = true;
    }

    Timer {
        id: formSubmitTimer;
        interval: 0;
        onTriggered: submitFormFromTimer();
    }

    function submitFormFromTimer() {
        validateInput();
        if (!formValid) {
            loadingOverlay.visible = false;
            return;
        }

        if(state == "login") {
            var password = loginForm.password;
            u1credservice.login(emailTextField.text, password);
        } else if(state == "register") {
            var password = registerForm.password;
            var display_name = registerForm.display_name;
            u1credservice.registerUser(emailTextField.text, password, display_name);
        } else if(state == "twofactor") {
            u1credservice.login(emailTextField.text, loginForm.password, loginForm.twoFactorCode);
        }
    }

    function handleSuccess() {
        loadingOverlay.visible = false;
        errorLabel.visible = false;
        account.updateDisplayName(emailTextField.text);
        account.updateEnabled(true);
        account.synced.connect(main.finished);
        account.sync();
        resetUI();
    }

    function showTwoFactorUI() {
        loadingOverlay.visible = false;
        if(state != "login") {
            console.log("Error: did not expect two factor request from register");
            showError("An internal error occurred. Please try again later.");
            return;
        }
        errorLabel.visible = false;
        state = "twofactor";
        loginForm.twoFactorVisible = true;
        formValid = false;
    }

    function validateInput() {
        formValid = emailTextField.acceptableInput;
        if(!formValid) {
            showError("Please enter a valid email address.");
            return;
        }

        if(state == "login" || state == "twofactor") {
            formValid &= loginForm.validateInput();
        } else {
            formValid &= registerForm.validateInput();
        }
        
        if(formValid) {
            errorLabel.visible = false;
        }
    }

    Component {
        id: accountServiceComponent
        AccountService {
            autoSync: false
        }
    }

    AccountServiceModel {
        id: accountServices
        includeDisabled: true
        account: __account.objectHandle
    }

    function enableAccount() {
        for (var i = 0; i < accountServices.count; i++) {
            var accountServiceHandle = accountServices.get(i, "accountService")
            var accountService = accountServiceComponent.createObject(null,
                                     { "objectHandle": accountServiceHandle })
            accountService.updateServiceEnabled(true)
            accountService.destroy(1000)
        }
    }

}
