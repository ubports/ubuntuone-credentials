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
import QtQuick 2.0
import Ubuntu.Components 0.1

Column {
    id: loginFormColumn
    spacing: units.gu(2)

    /* export two aliases for KeyNavigation access*/
    property alias passwordTextField: passwordTextField
    property alias twoFactorTextField: twoFactorTextField

    property alias password: passwordTextField.text
    property alias twoFactorVisible: twoFactorUI.visible
    property alias twoFactorCode: twoFactorTextField.text

    Label {
        text: i18n.dtr(rootFlickable.domain, "Enter your password:")
    }

    TextField {
        id: passwordTextField
        objectName: "loginFormPasswordTextField"
        echoMode: TextInput.Password
        width: main.width - (2 * main.anchors.margins)

        Keys.onReturnPressed: main.processForm();
        KeyNavigation.tab: twoFactorUI.visible ? twoFactorTextField : emailTextField /* from main */
        KeyNavigation.backtab: emailTextField /* from main */
        inputMethodHints: Qt.ImhSensitiveData
    }

    Label {
        text: '<a href="https://login.ubuntu.com/+forgot_password"><span style="color: #dd4814;">%1</span></a>'.arg(i18n.dtr(rootFlickable.domain, "Forgot password?"))
        textFormat: Text.RichText
        onLinkActivated: { Qt.openUrlExternally(link); }
    }

    Column {
        id: twoFactorUI
        spacing: units.gu(2)
        width: parent.width
        visible: false

        Label {
            text: i18n.dtr(rootFlickable.domain, "Type your verification code:")
        }

        TextField {
            id: twoFactorTextField
            objectName: "twoFactorTextField"
            placeholderText: i18n.dtr(rootFlickable.domain, "2-factor device code")
            inputMethodHints: Qt.ImhDigitsOnly
            width: main.width - (2 * main.anchors.margins)
            Keys.onReturnPressed: {
                main.processForm();
            }
            KeyNavigation.tab: emailTextField
            KeyNavigation.backtab: passwordTextField
        }

        Label {
            text: '<a href="https://login.ubuntu.com/+device-help"><span style="color: #dd4814;">%1</span></a>'.arg(i18n.dtr(rootFlickable.domain, "Authentication Device Help"))
            textFormat: Text.RichText
            onLinkActivated: { Qt.openUrlExternally(link); }
        }
    }

    Row {
        objectName: "buttonRow-LoginForm"
        height: units.gu(5)
        spacing: units.gu(1)
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            id: btnCancel
            objectName: "cancelButton"
            text: i18n.dtr(rootFlickable.domain, "Cancel")
            color: "#1c091a"
            height: parent.height
            width: (parent.width / 2) - 0.5 * parent.spacing
            onClicked: {
                userCancelled();
            }
        }

        Button {
            id: btnContinue
            objectName: "continueButton"
            text: i18n.dtr(rootFlickable.domain, "Sign in")
            color: "#cc3300"
            height: parent.height
            width: (parent.width / 2) - 0.5 * parent.spacing
            onClicked: {
                processForm();
            }
        }
    }

    Label {
        id: toggleLabel
        objectName: "loginFormToggleLabel"
        text: '<a href="#"><span style="color: #dd4814;">%1</span</a>'.arg(i18n.dtr(rootFlickable.domain, "I'm a new Ubuntu One user, sign me up"))

        textFormat: Text.RichText
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
        onLinkActivated: { toggleNewUser(); }
    }

    function resetUI() {
        twoFactorUI.visible = false;
        twoFactorTextField.text = "";
        passwordTextField.text = "";
    }

    function validateInput() {
        var passwordLongEnough = passwordTextField.text.length > 7;
        passwordTextField.errorHighlight = !passwordLongEnough;
        if (!passwordLongEnough) {
            main.showError(i18n.dtr(rootFlickable.domain, "Password must be at least 8 characters long."));
        }

        if (!twoFactorUI.visible) {
            return passwordLongEnough;
        }

        var twoFactorLongEnough = twoFactorTextField.text.length > 0;
        twoFactorTextField.errorHighlight = !twoFactorLongEnough;
        if(!twoFactorLongEnough){
            main.showError(i18n.dtr(rootFlickable.domain, "Please enter your two-factor device code."));
        }
        return passwordLongEnough && twoFactorLongEnough;
    }
}
