import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    // Set a non-zero height and width so that the parent Column in
    // CredentialsUI lays out correctly. Set matching color so we
    // don't see a line.
    height: 0.001
    width: main.width
    color: main.color 

    property alias password: passwordTextField.text
    property alias twoFactorVisible: twoFactorUI.visible
    property alias twoFactorCode: twoFactorTextField.text


    Column {
        spacing: units.gu(2)

        Label{
            text: "I am a returning user and my password is:"
            fontSize: "large"
            wrapMode: Text.WordWrap// WrapAtWordBoundary
        }
        TextField {
            id: passwordTextField

            placeholderText: "Your password"
            echoMode: TextInput.Password
            width: main.width - (2 * main.anchors.margins)

            //todo: fix tab nav?
            // KeyNavigation.backtab: txtEmail
            // KeyNavigation.tab: txtEmail
            Keys.onReturnPressed: main.process_form();
        }

        Label{
            text: '<a href="https://login.ubuntu.com/+forgot_password"><span style="color: #dd4814;">Forgotten your password?</span></a>'
            textFormat: Text.RichText
            fontSize: "small"
        }

        
        Column{
            id: twoFactorUI
            spacing: units.gu(2)
            height: 1
            width: parent.width
            visible: false

            Label{
                text: "Type your verification code:"
                fontSize: "large"
            }

            TextField {
                id: twoFactorTextField
                placeholderText: "2-factor device code"
                echoMode: TextInput.Password
                focus: true
                width: main.width - (2 * main.anchors.margins)
                Keys.onReturnPressed: {
                    main.process_form();
                }
            }

            Label{
                text: '<a href="https://login.ubuntu.com/+device-help"><span style="color: #dd4814;">Authentication Device Help</span></a>'
                textFormat: Text.RichText
                fontSize: "small"

                wrapMode: Text.WordWrap
            }

        } // Rectangle : twoFactorUI


    }// Column

}
