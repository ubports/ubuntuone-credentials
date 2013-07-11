import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    // Set a non-zero height and width so that the parent Column in
    // CredentialsUI lays out correctly. Set matching color so we
    // don't see a 1px line.
    height: 0.001
    width: main.width
    color: main.color

    property alias password: txtPassword.text
    property alias display_name: txtName.text


    Column {
        // anchors.fill: parent
        spacing: units.gu(2)

        Label {
            id: subtitle
            fontSize: "large"
            text: "Please tell us your name and choose a password."
            wrapMode: Text.WordWrap// WrapAtWordBoundary
        }
        
        TextField {
            id: txtName
            placeholderText: "Your name"
            width: main.width - 2 * mainColumn.spacing
        }

        TextField {
            id: txtPassword
            placeholderText: "Password with at least 8 characters"
            echoMode: TextInput.Password
            width: main.width - 2 * mainColumn.spacing
        }

        TextField {
            id: txtConfirmPassword
            placeholderText: "Re-type password"
            echoMode: TextInput.Password
            width: main.width - 2 * mainColumn.spacing
        }
        Row{
            spacing: units.gu(2)
            CheckBox {
                id: termsAndConditionsCheckBox
                checked: false
            }

            Label{
                anchors.verticalCenter: termsAndConditionsCheckBox.verticalCenter
                text: "I agree to the <a href='http://one.ubuntu.com/terms/'>Ubuntu One Terms and Conditions</a>"
                fontSize: "medium"
            }
        }        
    }

}
