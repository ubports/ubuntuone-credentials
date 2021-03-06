/*
 * Copyright (C) 2013-2014 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import UbuntuOne 1.0
import Ubuntu.OnlineAccounts 0.1


Column {
    id: main

    property variant __account: account

    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm
    property var formValid: false

    signal finished

    anchors.margins: units.gu(2)

    spacing: units.gu(2)

    Component.onCompleted: {
        resetUI();
    }

    Label {
        id: topLabel
        objectName: "topLabel"
        text: ""
        font.pixelSize: units.gu(2.5)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(2)
        wrapMode: Text.Wrap
    }

    Label {
        id: errorLabel
        objectName: "errorLabel"
        text: ""
        font.bold: true
        color: "red"
        visible: false

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(4)
        wrapMode: Text.Wrap
    }

    Label {
        text: i18n.dtr(rootFlickable.domain, "Please type your email:")
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(2)
    }

    TextField {
        id: emailTextField
        objectName: "emailTextField"
        placeholderText: i18n.dtr(rootFlickable.domain, "example: user@ubuntu.com")
        width: main.width - (2 * main.anchors.margins)
        validator: RegExpValidator { regExp: /.+@.+/ }
        focus: true
        inputMethodHints: Qt.ImhEmailCharactersOnly
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: units.gu(2)
        onLengthChanged: {
            if (emailTextField.length > 0 && main.state == "register") {
                validateInput();
            }
        }
    }

    LoginForm {
        id: loginForm
        objectName: "loginForm"
        visible: true
        
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    RegisterForm {
        id: registerForm
        objectName: "registerForm"
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
                errorMessage = i18n.dtr(rootFlickable.domain, "Please enter a valid email address.");
            }
            showError(errorMessage);
        }
    }

    function resetUI() {
        errorLabel.visible = false;
        errorLabel.text = "";
        emailTextField.text = "";
        loginForm.resetUI()
        registerForm.resetUI();
        state = "register";
        toggleNewUser();
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
        if(state == "login" || state == "twofactor") {
            topLabel.text = i18n.dtr(rootFlickable.domain, "Create your Ubuntu One account")
            switchTo(registerForm)
            state = "register";
            registerForm.nameTextField.focus = true;
        } else if(state == "register") {
            topLabel.text = i18n.dtr(rootFlickable.domain, "Sign in to your Ubuntu One account")
            switchTo(loginForm)
            state = "login";
            loginForm.passwordTextField.focus = true;
        } else {
            console.debug("unexpected state" + state + "in toggleNewUser");
        }
    }

    function userCancelled() {
        console.debug("Removing account ID: " + account.accountId);
        account.removed.connect(finished);
        account.remove(Account.RemoveCredentials);
    }

    /* processForm uses a timer to delay calling u1credservice, which can
       take a while, to ensure that the loadingOverlay is shown
       immediately.
       */
    function processForm() {
        Qt.inputMethod.hide();
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
        main.finished();
    }

    function showTwoFactorUI() {
        loadingOverlay.visible = false;
        if(state != "login") {
            console.log("Error: did not expect two factor request from register");
            showError(i18n.dtr(rootFlickable.domain, "An internal error occurred. Please try again later."));
            return;
        }
        errorLabel.visible = false;
        state = "twofactor";
        loginForm.twoFactorVisible = true;
        loginForm.twoFactorTextField.focus = true;
        formValid = false;
    }

    function validateInput() {
        formValid = emailTextField.acceptableInput;
        if(!formValid) {
            showError(i18n.dtr(rootFlickable.domain, "Please enter a valid email address."));
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
}
