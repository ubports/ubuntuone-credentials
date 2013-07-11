import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0


Rectangle{ 
    id: main
    width: parent.width
    anchors.margins: units.gu(2)

    state: "login" // or "register" or "twofactor"
    property var currentVisible: loginForm

    color: "white"


    Column{
        id: mainColumn
        spacing: units.gu(4)
        width: parent.width
        anchors.margins: parent.anchors.margins

        add: Transition{
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

        Label{
            id: errorLabel
            text: ""
            font.bold: true
            color: "red"
            visible: false

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins

        }

        Label{
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
        }

        Row{
            spacing: units.gu(2)
            Switch {
                id: newUserToggleSwitch
                checked: false

                onCheckedChanged: {
                    toggleNewUser();
                }
            }
            Label{
                anchors.verticalCenter: newUserToggleSwitch.verticalCenter
                text: "I am a new Ubuntu One user"
                fontSize: "large"
            }
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins

        } // Row


        LoginForm{
            id: loginForm
            visible: true
            
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: parent.anchors.margins
        }
        
        RegisterForm{ 
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
                // todo
            }
        }
        Button {
            id: btnContinue
            text: "Continue"
            color: "#cc3300"
            height: parent.height
            width: (parent.width / 2) - parent.anchors.margins - parent.spacing

            onClicked: { 
                process_form();
            }

        }
    }


    Rectangle {
        id: loadingOverlay
        opacity: 0.6
        visible: false

        ActivityIndicator {
            id: activity
            anchors.centerIn: parent
            running: true
        }
        Button { 
            id: cancelLoadingButton
            text: "Cancel"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: activity.bottom
            onClicked: {console.log("Cancel clicked, does nothing");}
        }
    }

    Rectangle {
        id: successOverlay
        opacity: 0.6
        visible: false

        Label { id: successLabel

                anchors.centerIn: parent
                font.bold: true
                fontSize: "x-large"
                color: "#dd4814"
                text: "Login Successful!"
              }
    }

    UbuntuOneCredentialsService {
        id: u1credservice

        /* NOTE: if you are reading this as example code, see
           examples/embeddingMain for handing querying for presence of
           creds and signing URLs without showing UI. This is not an
           exhaustive list of supported signals. */

        onLoginOrRegisterSuccess: {
            console.log(" login success.");
            login_successful();
        }

        onTwoFactorAuthRequired: {
            showTwoFactorUI();
        }

        onLoginOrRegisterError: {
            showError(errorMessage);
        }
    }
    
    Component.onCompleted: { 
        switchTo(loginForm);
        console.log("main width:" + width);
    }

    function showError(message){
        errorLabel.text = message;
        errorLabel.visible = true;
        loadingOverlay.visible = false;
    }

    function switchTo(newVisible){
        currentVisible.visible = false;
        newVisible.visible = true;
        currentVisible = newVisible;
    }

    function toggleNewUser(){
        if(state == "login"){
            switchTo(registerForm)
            state = "register";
        }else if(state == "register"){
            switchTo(loginForm)
            state = "login";
        }else{
            console.debug("unexpected state" + state + "in toggleNewUser");
        }
    }

    function process_form(){
        loadingOverlay.visible = true;
        if(state == "login"){
            var password = loginForm.password;
            u1credservice.login(emailTextField.text, password);
        }else if(state == "register"){
            var password = registerForm.password;
            var display_name = registerForm.display_name;
            u1credservice.registerUser(emailTextField.text, password, display_name);
        }else if(state == "twofactor"){
           u1credservice.login(emailTextField.text, loginForm.password, loginForm.twoFactorCode);
        }
    }

    function login_successful(){
        btnContinue.visible = false;
        loadingOverlay.visible = false;
        successOverlay.visible = true;
        errorLabel.visible = false;
    }

    function showTwoFactorUI(){
        loadingOverlay.visible = false;
        if(state != "login"){
            console.log("Error: did not expect two factor request from register");
            return;
        }
        state = "twofactor";
        loginForm.twoFactorVisible = true;
            // todo: text focus?
    }

}
