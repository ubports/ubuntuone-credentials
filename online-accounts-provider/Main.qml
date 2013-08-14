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
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.OnlineAccounts 0.1


Column {
    id: main

    /* To override the parameters coming from the .provider file: */
    property variant authenticationParameters: {"foo": "bar"}

    property variant authReply
    property bool isNewAccount: false
    property variant __account: account
    property alias globalAccountService: globalAccountSettings


    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm
    property var formValid: false;

    signal authenticated(variant reply)
    signal authenticationError(variant error)
    signal finished

    anchors.left: parent.left
    anchors.right: parent.right

    Component.onCompleted: {
        isNewAccount = (account.accountId === 0)
        enableAccount()
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
        /*temp*/ text: "michael.mccracken@gmail.com"
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

    // Non-visual stuff -------------------------------------------------

    Credentials {
        id: creds
        caption: account.provider.id
        storeSecret: false
        acl: ["*"]
        onCredentialsIdChanged: main.credentialsStored()
    }

    AccountService {
        id: globalAccountSettings
        objectHandle: account.accountServiceHandle
        credentials: creds
        autoSync: false

        onAuthenticated: {
            console.log("in AccountService globalAccountSettings onAuthenticated")
            authReply = reply
            main.authenticated(reply)
        }
        onAuthenticationError: main.authenticationError(error)
    }

    AccountServiceModel {
        id: accountServices
        includeDisabled: true
        account: __account.objectHandle
    }

    Component {
        id: accountServiceComponent
        AccountService {
            autoSync: false
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

        creds.userName = emailTextField.text;
        main.authenticationParameters['test_state'] = state;
        console.log("in processForm, state is " + state);
        console.log("in processForm, main.authenticationParameters is " + main.authenticationParameters);

        if(state == "login") {
            creds.secret = loginForm.password;

        } else if(state == "register") {
            creds.secret = registerForm.password;
            main.authenticationParameters['display_name'] = registerForm.display_name;
//            u1credservice.registerUser(emailTextField.text, password, display_name);

        } else if(state == "twofactor") {
            creds.secret = loginForm.password
            main.authenticationParameters['two_factor_code'] = loginForm.twoFactorCode;
  //          u1credservice.login(emailTextField.text, loginForm.password, loginForm.twoFactorCode);
        }

        creds.sync();

    }

    // credentialsStored is emitted in response to a successful sync(), which creates the signon Identity
    function credentialsStored() {
        console.log("Credentials stored, id: " + creds.credentialsId)
        if (creds.credentialsId == 0) return
        globalAccountSettings.authenticate(main.authenticationParameters)
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

// -------------------------------------------------------------------------------- OLD:
    function authenticate() {
        console.log("Authenticating...")
        creds.sync()
    }

    function cancel() {
        if (isNewAccount && creds.credentialsId != 0) {
            console.log("Removing credentials...")
            creds.remove()
            creds.removed.connect(finished)
        } else {
            finished()
        }
    }

    function enableAccount() {
        for (var i = 0; i < accountServices.count; i++) {
            var accountServiceHandle = accountServices.get(i, "accountService")
            var accountService = accountServiceComponent.createObject(null,
                                     { "objectHandle": accountServiceHandle })
            accountService.updateServiceEnabled(true)
            accountService.destroy(1000)
        }
        globalAccountSettings.updateServiceEnabled(true)
    }

    /* reimplement this function in plugins in order to perform some actions
     * before quitting the plugin */
    function completeCreation(reply) {
        var userName = getUserName(reply)

        console.log("UserName: " + userName)
        if (userName != '') account.updateDisplayName(userName)
        account.synced.connect(finished)
        account.sync()
    }

    onAuthenticated: completeCreation(reply)

    onAuthenticationError: main.cancel()
}
