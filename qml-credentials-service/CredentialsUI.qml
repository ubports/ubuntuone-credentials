import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0

Rectangle {
    id: main
    width: 400
    height: 600
    gradient: Gradient {
         GradientStop { position: 0.0; color: "#863636" }
         GradientStop { position: 0.5; color: "#5b2235" }
         GradientStop { position: 1.0; color: "#2b0f21" }
     }

    property string state: "login" // or "register" or "twofactor"

     UbuntuOneCredentialsService {
         id: u1credservice
 
         onCredentialsFound: {
             console.log("Credentials Found");
             signUrl("http://server", "GET");
         }

         onLoginOrRegisterSuccess: {
             console.log(" login success.");
             signUrl("http://server", "GET");
             login_successful();
         }


        onUrlSigned: {
                console.log("signed url is "+ signedUrl);
        }

        onUrlSigningError: {
                console.log("signing error.");
                console.log(errorMessage);
         }

         onCredentialsNotFound: {
             console.log("\nCredentials NOT FOUND");
                     signUrl("http://server", "GET"); // xfail
         }

         onTwoFactorAuthRequired: {
                 console.log("Account requires two-factor code.");
                 showTwoFactorForm();
         }

         onLoginOrRegisterError: {
             console.log("getting credentials failed");
                 console.log(errorMessage);
             error();
         }
     }
 
    Label {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: units.gu(1)
        fontSize: "x-large"
        color: "#e5d8ce"
        text: "Ubuntu Sign In"
    }

    UbuntuShape {
        anchors.left: parent.left
        anchors.top: title.bottom
        anchors.topMargin: units.gu(3)
        width: parent.width
        height: parent.height - y - buttons.height - units.gu(2)

        LoginForm {
            id: loginForm
            x: 0
            anchors.top: parent.top
            width: parent.width

            Behavior on x { PropertyAnimation { duration: 300 } }
        }

        RegisterForm {
            id: registerForm
            x: parent.width
            anchors.top: parent.top
            width: parent.width

            Behavior on x { PropertyAnimation { duration: 300 } }
        }

        TwoFactorForm {
                id: twoFactorForm
                x: parent.width
                anchors.top: parent.top
                width: parent.width

                Behavior on x { PropertyAnimation { duration: 300 } }
        }


        SuccessScreen {
            id: successScreen
            x: parent.width
            anchors.top: parent.top
            width: parent.width

            Behavior on x { PropertyAnimation { duration: 300 } }
        }
    }

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
            text: "Check existing creds"
            color: "#1c091a"
            height: parent.height
            width: (parent.width / 2) - parent.anchors.margins - parent.spacing
            onClicked: {
                    console.debug("checking for creds in keyring:");
                    u1credservice.checkCredentials();
                }
        }
        Button {
            id: btnContinue
            text: "Continue"
            color: "#cc3300"
            height: parent.height
            width: (parent.width / 2) - parent.anchors.margins - parent.spacing

            onClicked: { 
                    console.log("Continue clicked")
                    process_form();
                }

        }
    }

    Rectangle {
        id: loading
        anchors.fill: parent
        opacity: 0.6
        visible: false

        ActivityIndicator {
            id: activity
            anchors.centerIn: parent
            running: true
        }
    }

    function toggleNewUser(){
        if(state == "login"){
            registerForm.new_switch.checked = loginForm.new_switch.checked;
            loginForm.x = -main.width;
            registerForm.x = 0;
            state = "register";
        }else if(state == "register"){
            loginForm.new_switch.checked = registerForm.new_switch.checked;
            loginForm.x = 0;
            registerForm.x = main.width;
            state = "login";
        }else{
            console.debug("unexpected state" + state + "in toggleNewUser");
        }

    }

    function process_form(){
        if(state == "login"){
            loading.visible = true;
            var email = loginForm.email;
            var password = loginForm.password;
            u1credservice.login(email, password);
        }else if(state == "register"){
            loading.visible = true;
            var email = registerForm.email;
            var password = registerForm.password;
            var display_name = registerForm.display_name;
            u1credservice.registerUser(email, password, display_name);
        }else if(state == "twofactor"){
            u1credservice.login(loginForm.email, loginForm.password, twoFactorForm.twoFactorCode);
        }
    }

    function login_successful(){
        if(state == "login"){
            loginForm.x = -main.width;
        }else if(state == "register"){
            registerForm.x = -main.width;
        }else if(state == "twofactor"){
            twoFactorForm.x = -main.width
        }else{
            console.debug("unexpected state " + state + " in login_successful");
        }
        successScreen.x = 0;
        btnContinue.visible = false;
        loading.visible = false;
    }

    function showTwoFactorForm(){
        loading.visible = false;
        if(state != "login"){
            console.log("Error: did not expect two factor request from register");
            return;
        }
        state = "twofactor";
        twoFactorForm.twoFactorTextField.focus = true;
        loginForm.x = -main.width;
        twoFactorForm.x = 0;
    }

    function error(){
        
        loading.visible = false;
    }
}
