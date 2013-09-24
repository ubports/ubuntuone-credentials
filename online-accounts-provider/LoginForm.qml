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
        text: "I am a returning user and my password is:"
    }

    TextField {
        id: passwordTextField
        objectName: "loginFormPasswordTextField"
        placeholderText: "Your password"
        echoMode: TextInput.Password
        width: main.width - (2 * main.anchors.margins)

        Keys.onReturnPressed: main.processForm();
        KeyNavigation.tab: twoFactorUI.visible ? twoFactorTextField : emailTextField /* from main */
        KeyNavigation.backtab: emailTextField /* from main */
    }

    Label {
        text: '<a href="https://login.ubuntu.com/+forgot_password"><span style="color: #dd4814;">Forgotten your password?</span></a>'
        textFormat: Text.RichText
        onLinkActivated: { Qt.openUrlExternally(link); }
    }

    ButtonRow {
        objectName: "buttonRow-LoginForm-noTwoFactor"
        anchors.left: parent.left
        anchors.right: parent.right

        visible: !twoFactorUI.visible
    }

    Column {
        id: twoFactorUI
        spacing: units.gu(2)
        width: parent.width
        visible: false

        Label {
            text: "Type your verification code:"
        }

        TextField {
            id: twoFactorTextField
            objectName: "twoFactorTextField"
            placeholderText: "2-factor device code"
            echoMode: TextInput.Password
            width: main.width - (2 * main.anchors.margins)
            Keys.onReturnPressed: {
                main.processForm();
            }
            KeyNavigation.tab: emailTextField
            KeyNavigation.backtab: passwordTextField
        }

        Label {
            text: '<a href="https://login.ubuntu.com/+device-help"><span style="color: #dd4814;">Authentication Device Help</span></a>'
            textFormat: Text.RichText
            onLinkActivated: { Qt.openUrlExternally(link); }
        }

        ButtonRow {
            anchors.left: parent.left
            anchors.right: parent.right
        }

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
