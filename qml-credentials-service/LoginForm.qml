import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "transparent"

    property alias new_switch: newSwitch
    property alias email: txtEmail.text
    property alias password: txtPassword.text

    Label {
        id: subtitle
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: units.gu(1)
        anchors.leftMargin: units.gu(2)
        fontSize: "large"
        color: "white"
        text: "Please type your email:"
    }

    Grid {
        anchors.fill: parent
        anchors.topMargin: units.gu(8)
        anchors.margins: units.gu(2)
        columns: 2
        spacing: units.gu(2)

        Label{
            text: "Your email"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtEmail
            placeholderText: "Your email"
            focus: true;
            KeyNavigation.tab: txtPassword
            KeyNavigation.backtab: txtPassword
        }

        Label{
            text: "I am a new\nUbuntu One user"
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
            text: "Your Password"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: txtPassword
            placeholderText: "Your password"
            echoMode: TextInput.Password
            KeyNavigation.backtab: txtEmail
            KeyNavigation.tab: txtEmail
            Keys.onReturnPressed: main.process_form();
        }

        Label{
            text: " "
        }
        Label{
            text: '<a href="http://google.com"><span style="color: #dd4814;">Forgotten your password?</span></a>'
            textFormat: Text.RichText
            color: "white"
            fontSize: "small"
            width: txtEmail.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

    }

}
