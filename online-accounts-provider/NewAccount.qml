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

    //property variant authenticationParameters: null

    property bool isNewAccount: false
    property variant __account: account
// unneeded:?    property alias globalAccountService: globalAccountSettings


    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm
    property var formValid: false

    signal finished

    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        isNewAccount = (account.accountId === 0);
        resetUI();
    }

    Label {
        id: title
        text: "One account to log in to everything on Ubuntu"
        fontSize: "medium"
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
        fontSize: "large"

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
    }

    TextField {
        id: emailTextField
        placeholderText: "Your email"
        focus: true;

        width: main.width - (2 * main.anchors.margins)
        anchors.left: parent.left
        anchors.margins: parent.anchors.margins
    }

    Row {
        spacing: units.gu(2)
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
            fontSize: "large"
        }
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins

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

    Rectangle {
        id: loadingOverlay
        opacity: 0.6
        color: "white"
        visible: false
        width: main.width
        height: main.height
        //TODO: anchors.centerIn: main.parent

        ActivityIndicator {
            id: activity
          //TODO:  anchors.centerIn: parent
            running: true
        }
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
        } else if(state == "register") {
            switchTo(loginForm)
            state = "login";
        } else {
            console.debug("unexpected state" + state + "in toggleNewUser");
        }
    }

    function processForm() {
        validateInput();
        if (!formValid) {
            return;
        }
        loadingOverlay.visible = true;
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
        resetUI();
        finished();
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

}