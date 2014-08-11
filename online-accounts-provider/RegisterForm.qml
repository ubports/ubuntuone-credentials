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
    id: registerFormColumn
    spacing: units.gu(2)

    /* export two aliases for KeyNavigation access*/
    property alias nameTextField: nameTextField
    property alias confirmPasswordTextField: confirmPasswordTextField

    property alias password: newPasswordTextField.text
    property alias display_name: nameTextField.text


    Label {
        id: subtitle
        text: i18n.dtr(rootFlickable.domain, "Please tell us your name:")
    }
    
    TextField {
        id: nameTextField
        objectName: "nameTextField"
        placeholderText: i18n.dtr(rootFlickable.domain, "First and last name")
        width: main.width - (2 * main.anchors.margins)
        KeyNavigation.tab: newPasswordTextField
        KeyNavigation.backtab: emailTextField
        onLengthChanged: {
            if (nameTextField.length > 0) {
                validateInput();
            }
        }
    }

    Label {
        id: passwordTitles
        text: i18n.dtr(rootFlickable.domain, "Choose a password:")
    }
    
    TextField {
        id: newPasswordTextField
        objectName: "newPasswordTextField"
        placeholderText: i18n.dtr(rootFlickable.domain, "At least 8 characters")
        echoMode: TextInput.Password
        width: main.width - (2 * main.anchors.margins)
        KeyNavigation.tab: confirmPasswordTextField
        KeyNavigation.backtab: nameTextField
        inputMethodHints: Qt.ImhSensitiveData
        onLengthChanged: {
            if (newPasswordTextField.length > 0) {
                validateInput();
            }
        }
    }

    TextField {
        id: confirmPasswordTextField
        objectName: "confirmPasswordTextField"
        placeholderText: i18n.dtr(rootFlickable.domain, "Must match the previous field")
        echoMode: TextInput.Password
        width: main.width - (2 * main.anchors.margins)
        KeyNavigation.tab: emailTextField
        KeyNavigation.backtab: newPasswordTextField
        inputMethodHints: Qt.ImhSensitiveData
        onLengthChanged: {
            if (confirmPasswordTextField.length > 0) {
                validateInput();
            }
        }
    }

    Row {
        spacing: units.gu(2)
        CheckBox {
            id: termsAndConditionsCheckBox
            objectName: "termsAndConditionsCheckBox"
            checked: false
            onClicked: {
                validateInput();
            }
        }

        Label {
            anchors.verticalCenter: termsAndConditionsCheckBox.verticalCenter
            // TRANSLATORS: DO NOT change the URL or color here.
            text: i18n.dtr(rootFlickable.domain, 'I agree to the <a href="https://one.ubuntu.com/terms/"><span style="color: #dd4814;">Ubuntu One T&amp;Cs</span></a>')
            onLinkActivated: { Qt.openUrlExternally(link); }
            textFormat: Text.RichText
        }
    }

    Row {
        objectName: "buttonRow-RegisterForm"
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
            text: i18n.dtr(rootFlickable.domain, "Continue")
            color: "#cc3300"
            height: parent.height
            width: (parent.width / 2) - 0.5 * parent.spacing
            onClicked: {
                processForm();
            }
            enabled: false
        }
    }

    Label {
        id: toggleLabel
        objectName: "toggleLabel"
        text: '<a href="#"><span style="color: #dd4814;">%1</span></a>'.arg(i18n.dtr(rootFlickable.domain, "I'm an existing Ubuntu One user, sign me in"))

        textFormat: Text.RichText
        horizontalAlignment: Text.AlignHCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: parent.anchors.margins
        onLinkActivated: { toggleNewUser(); }
    }

    function resetUI() {
        termsAndConditionsCheckBox.checked = false;
        nameTextField.text = "";
        newPasswordTextField.text = "";
        confirmPasswordTextField.text = "";
        btnContinue.enabled = false;
    }
    
    function validateInput() {

        var nameOK = (nameTextField.text != "");
        nameTextField.errorHighlight = !nameOK;
        if (!nameOK) {
            main.showError(i18n.dtr(rootFlickable.domain, "Please enter a name."));
            return false;
        }

        var passwordLongEnough = newPasswordTextField.length > 7;
        newPasswordTextField.errorHighlight = !passwordLongEnough;
        if (!passwordLongEnough) {
            main.showError(i18n.dtr(rootFlickable.domain, "Your password must be at least 8 characters long."));
            return false;
        }

        var passwordsMatch = (newPasswordTextField.text == confirmPasswordTextField.text);
        confirmPasswordTextField.errorHighlight = !passwordsMatch;
        if (!passwordsMatch) {
            main.showError(i18n.dtr(rootFlickable.domain, "The passwords do not match."));
            return false;
        }
        
        if (!termsAndConditionsCheckBox.checked) {
            main.showError(i18n.dtr(rootFlickable.domain, "Please accept the terms and conditions by checking the box."));
            return false;
        }

        btnContinue.enabled = true;
        errorLabel.visible = false;
        return true;
    }
}
