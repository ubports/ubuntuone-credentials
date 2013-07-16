import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0

Rectangle {
    id: main
    width: parent.width
    anchors.margins: units.gu(2)

    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm
    property var formValid: false;

    color: "white"

    signal userCancelled()
    signal succeeded()

    Column {
        id: mainColumn
        spacing: parent.anchors.margins
        width: parent.width
        anchors.margins: parent.anchors.margins

        add: Transition {
            NumberAnimation { properties: "opacity"; easing.type: Easing.OutQuad; duration: 1000}
        }

        Label {
            id: title
            text: "One account to log in to\neverything on Ubuntu"
            fontSize: "x-large"
            color: UbuntuColors.coolGrey
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins
        }

        Label {
            id: errorLabel
            text: ""
            font.bold: true
            color: "red"
            visible: false

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins

        }

        Label {
            text: "Please type your email:"
            fontSize: "large"

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins
        }

        TextField {
            id: emailTextField
            
            placeholderText: "Your email"
            focus: true;

            width: main.width - (2 * main.anchors.margins)
            anchors.left: parent.left
            anchors.margins: parent.anchors.margins
            validator: RegExpValidator{ regExp: /.*@.*/ }
        }

        Row {
            spacing: units.gu(2)
            Switch {
                id: newUserToggleSwitch
                checked: false

                onCheckedChanged: {
                    toggleNewUser();
                }
            }
            Label {
                anchors.verticalCenter: newUserToggleSwitch.verticalCenter
                text: "I am a new Ubuntu One user"
                fontSize: "large"
            }
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins

        } // Row


        LoginForm {
            id: loginForm
            visible: true
            
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins
        }

        RegisterForm {
            id: registerForm
            visible: false
            
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins
        }


    } // Column

    Row {
        id: buttons
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: units.gu(1)
        width: parent.width
        height: units.gu(5)
        spacing: units.gu(1)

        Button {
            id: btnCancel
            text: "Cancel"
            color: "#1c091a"
            height: parent.height
            width: (parent.width / 2) - parent.anchors.margins - parent.spacing
            onClicked: {
                main.userCancelled();
            }
        }
        Button {
            id: btnContinue
            text: "Continue"
            color: "#cc3300"
            height: parent.height
            width: (parent.width / 2) - parent.anchors.margins - parent.spacing
            onClicked: {
                processForm();
            }
        }
    }


    Rectangle {
        id: loadingOverlay
        opacity: 0.6
        color: "white"
        visible: false
        width: main.width
        height: main.height
        anchors.centerIn: main

        ActivityIndicator {
            id: activity
            anchors.centerIn: parent
            running: true
        }
    }

    UbuntuOneCredentialsService {
        id: u1credservice

        /* NOTE: if you are reading this as example code, see
           examples/embeddingMain for an example of querying for
           presence of creds and signing URLs without showing UI. This
           is not an exhaustive list of supported signals. */

        onLoginOrRegisterSuccess: {
            handleSuccess();
        }

        onTwoFactorAuthRequired: {
            showTwoFactorUI();
        }

        onLoginOrRegisterError: {
            if (errorMessage == "Invalid request data") {
                errorMessage = "Please check your input and try again.";
            }
            showError(errorMessage);
        }
    }
    
    Component.onCompleted: {
        resetUI();
    }

    function resetUI() {
        errorLabel.visible = false;
        emailTextField.text = "";
        loginForm.resetUI()
        registerForm.resetUI();
        newUserToggleSwitch.checked = false;
        state = "login";
        switchTo(loginForm);
        formValid = false;
    }

    function showError(message) {
        errorLabel.text = message;
        errorLabel.visible = true;
        loadingOverlay.visible = false;
    }

    function switchTo(newVisible) {
        currentVisible.visible = false;
        newVisible.visible = true;
        currentVisible = newVisible;
    }

    function toggleNewUser() {
        if(state == "login") {
            switchTo(registerForm)
            state = "register";
        } else if(state == "register") {
            switchTo(loginForm)
            state = "login";
        } else {
            console.debug("unexpected state" + state + "in toggleNewUser");
        }
    }

    function processForm() {
        validateInput();
        if (!formValid) {
            return;
        }
        loadingOverlay.visible = true;
        if(state == "login") {
            var password = loginForm.password;
            u1credservice.login(emailTextField.text, password);
        } else if(state == "register") {
            var password = registerForm.password;
            var display_name = registerForm.display_name;
            u1credservice.registerUser(emailTextField.text, password, display_name);
        } else if(state == "twofactor") {
            u1credservice.login(emailTextField.text, loginForm.password, loginForm.twoFactorCode);
        }
    }

    function handleSuccess() {
        loadingOverlay.visible = false;
        errorLabel.visible = false;
        resetUI();
        main.succeeded();
    }

    function showTwoFactorUI() {
        loadingOverlay.visible = false;
        if(state != "login") {
            console.log("Error: did not expect two factor request from register");
            showError("An internal error occurred. Please try again later.");
            return;
        }
        errorLabel.visible = false;
        state = "twofactor";
        loginForm.twoFactorVisible = true;
        formValid = false;
    }

    function validateInput() {
        formValid = emailTextField.acceptableInput;
        if(!formValid) {
            showError("Please enter a valid email address.");
            return;
        }

        if(state == "login" || state == "twofactor") {
            formValid &= loginForm.validateInput();
        } else {
            formValid &= registerForm.validateInput();
        }
        
        if(formValid) {
            errorLabel.visible = false;
        }
    }

}
