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
        }

        Label{
            text: " "
        }
        Label{
            text: "Enter the email address you use to log into your Ubuntu One account. If you don't have an account yet, don't worry we'll create one for you."
            color: "white"
            fontSize: "small"
            width: txtEmail.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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
                main.switch_form();
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
        }

        Label{
            text: " "
        }
        Label{
            text: '<a href="https://login.ubuntu.com/+forgot_password"><span style="color: #dd4814;">Forgotten your password?</span></a>'
            textFormat: Text.RichText
            color: "white"
            fontSize: "small"
            width: txtEmail.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            onLinkActivated: { Qt.openUrlExternally(link); }
        }

    }

}
