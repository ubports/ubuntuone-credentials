import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    /* Set a non-zero height and width so that the parent Column lays out
       correctly. Set matching color so we don't see a 1px line. */

    height: 0.001
    width: main.width
    color: main.parent.color

    property alias password: passwordTextField.text
    property alias display_name: nameTextField.text


    Column {
        spacing: units.gu(2)

        Label {
            id: subtitle
            fontSize: "small"
            text: "Please tell us your name and choose a password."
        }
        
        TextField {
            id: nameTextField
            placeholderText: "Your name"
            width: main.width - (2 * main.anchors.margins)
        }

        TextField {
            id: passwordTextField
            placeholderText: "Password with at least 8 characters"
            echoMode: TextInput.Password
            width: main.width - (2 * main.anchors.margins)
        }

        TextField {
            id: confirmPasswordTextField
            placeholderText: "Re-type password"
            echoMode: TextInput.Password
            width: main.width - (2 * main.anchors.margins)
        }
        Row {
            spacing: units.gu(2)
            CheckBox {
                id: termsAndConditionsCheckBox
                checked: false
            }

            Label {
                anchors.verticalCenter: termsAndConditionsCheckBox.verticalCenter
                text: "I agree to the <a href='http://one.ubuntu.com/terms/'>Ubuntu One Terms and Conditions</a>"
                fontSize: "small"
                onLinkActivated: { Qt.openUrlExternally(link); }
            }
        }

        ButtonRow {
            anchors.left: parent.left
            anchors.right: parent.right
        }

    }

    function resetUI() {
        termsAndConditionsCheckBox.checked = false;
        nameTextField.text = "";
        passwordTextField.text = "";
        confirmPasswordTextField.text = "";
    }
    
    function validateInput() {
        var valid = termsAndConditionsCheckBox.checked;
        if (!valid) {
            main.showError("Please accept the terms and conditions by checking the box.");
            return false;
        }

        var nameOK = (nameTextField.text != "");
        nameTextField.errorHighlight = !nameOK;
        if (!nameOK) {
            main.showError("Please enter a name.");
            return false;
        }

        var passwordLongEnough = passwordTextField.length > 7;
        passwordTextField.errorHighlight = !passwordLongEnough;
        if (!passwordLongEnough) {
            main.showError("Your password must be at least 8 characters long.");
            return false;
        }

        var passwordsMatch = (passwordTextField.text == confirmPasswordTextField.text);
        confirmPasswordTextField.errorHighlight = !passwordsMatch;
        if (!passwordsMatch) {
            main.showError("The passwords do not match.");
            return false;
        }

        return true;
    }
}
