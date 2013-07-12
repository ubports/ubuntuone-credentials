import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "transparent"

    property alias twoFactorCode: twoFactorTextField.text
    property alias twoFactorTextField: twoFactorTextField

    Label {
        id: subtitle
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: units.gu(1)
        anchors.leftMargin: units.gu(2)
        fontSize: "large"
        color: "white"
        text: "2-Factor Authentication"
        width: parent.width
    }

    Grid {
        anchors.fill: parent
        anchors.topMargin: units.gu(8)
        anchors.margins: units.gu(2)
        columns: 1
        spacing: units.gu(2)

        Label{
            text: "Type your verification code:"
            color: "white"
            fontSize: "large"
        }
        TextField {
            id: twoFactorTextField
            placeholderText: "2-factor device code"
            echoMode: TextInput.Password
            focus: true
            Keys.onReturnPressed: {
                main.process_form();
            }
        }

        Label{
            text: '<a href="http://login.ubuntu.com/"><span style="color: #dd4814;">Authentication Device Help</span></a>'
            textFormat: Text.RichText
            color: "white"
            fontSize: "small"
            width: twoFactorTextField.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

    }

}
