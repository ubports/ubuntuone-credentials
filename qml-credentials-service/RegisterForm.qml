import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "transparent"

    property alias new_switch: newSwitch
    property alias email: txtEmail.text
    property alias password: txtPassword.text
    property alias display_name: txtName.text

    Label {
        id: subtitle
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: units.gu(1)
        anchors.leftMargin: units.gu(2)
        fontSize: "large"
        color: "white"
        text: "Please tell us your name and choose a password"
        width: parent.width
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    Grid {
        anchors.fill: parent
        anchors.topMargin: units.gu(10)
        anchors.margins: units.gu(1)
        columns: 2
        spacing: units.gu(1)

        Label{
            text: "Your email"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtEmail
            placeholderText: "Your email"
        }

        Label{
            text: "Your name"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtName
            placeholderText: "Your name"
        }

        Label{
            id: lblChoose
            text: "Choose a password"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtPassword
            placeholderText: "At list 8 characters"
            echoMode: TextInput.Password
        }

        Label{
            text: "Confirm password"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtConfirmPassword
            placeholderText: "Re-type your password"
            echoMode: TextInput.Password
        }

        Label{
            text: "New Customer"
            color: "white"
            fontSize: "large"
        }
        Switch {
            id: newSwitch
            checked: false

            onCheckedChanged: {
                main.toggleNewUser();
            }
        }

        Label{
            text: "I agree to the Ubuntu One Terms and Conditions"
            color: "white"
            fontSize: "medium"
            width: lblChoose.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        CheckBox {
            checked: true
        }

    }

}
