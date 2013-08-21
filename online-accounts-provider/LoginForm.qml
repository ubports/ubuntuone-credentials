import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    // Set a non-zero height and width so that the parent Column in
    // CredentialsUI lays out correctly. Set matching color so we
    // don't see a line.
    height: 0.001
    width: main.width
    color: main.parent.color 

    property alias password: passwordTextField.text
    property alias twoFactorVisible: twoFactorUI.visible
    property alias twoFactorCode: twoFactorTextField.text

    Column {
        spacing: units.gu(2)

        Label {
            text: "I am a returning user and my password is:"
            fontSize: "small"
        }
        TextField {
            id: passwordTextField

            placeholderText: "Your password"
            echoMode: TextInput.Password
            width: main.width - (2 * main.anchors.margins)

            Keys.onReturnPressed: main.processForm();
        }

        Label {
            text: '<a href="https://login.ubuntu.com/+forgot_password"><span style="color: #dd4814;">Forgotten your password?</span></a>'
            textFormat: Text.RichText
            fontSize: "small"
            onLinkActivated: { Qt.openUrlExternally(link); }
        }

        
        Column {
            id: twoFactorUI
            spacing: units.gu(2)
            height: 1
            width: parent.width
            visible: false

            Label {
                text: "Type your verification code:"
                fontSize: "small"
            }

            TextField {
                id: twoFactorTextField
                placeholderText: "2-factor device code"
                echoMode: TextInput.Password
                focus: true
                width: main.width - (2 * main.anchors.margins)
                Keys.onReturnPressed: {
                    main.processForm();
                }
            }

            Label {
                text: '<a href="https://login.ubuntu.com/+device-help"><span style="color: #dd4814;">Authentication Device Help</span></a>'
                textFormat: Text.RichText
                fontSize: "small"
                onLinkActivated: { Qt.openUrlExternally(link); }
            }

        } // Rectangle : twoFactorUI


    }// Column

    function resetUI() {
        twoFactorUI.visible = false;
        twoFactorTextField.text = "";
        passwordTextField.text = "";
    }

    function validateInput() {
        var passwordLongEnough = passwordTextField.text.length > 7;
        passwordTextField.errorHighlight = !passwordLongEnough;
        if (!passwordLongEnough) {
            main.showError("Password must be at least 8 characters long.");
        }

        if (!twoFactorUI.visible) {
            return passwordLongEnough;
        }
        
        var twoFactorLongEnough = twoFactorTextField.text.length > 0;
        twoFactorTextField.errorHighlight = !twoFactorLongEnough;
        if(!twoFactorLongEnough){
            main.showError("Please enter your two-factor device code.");
        }
        return passwordLongEnough && twoFactorLongEnough;
    }
}
