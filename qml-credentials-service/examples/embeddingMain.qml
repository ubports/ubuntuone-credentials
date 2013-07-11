import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0

Rectangle {
    id: exampleMain
    width: units.gu(100)
    height: units.gu(100)
    color: UbuntuColors.coolGrey

    Column {
        spacing: units.gu(2)
        anchors.fill: parent

        add: Transition{
            NumberAnimation { properties: "opacity"; easing.type: Easing.OutQuad; duration: 1000}
        }

        Row {
            spacing: units.gu(2)
            Label {
                id: label
                color: "white"
                text: "Show Creds UI"
                anchors.verticalCenter: showCredsUISwitch.verticalCenter
            }

            Switch {
                id: showCredsUISwitch

                checked: true
            }
        }

        // Here is where we've embedded the UI for UbuntuOne login and register.
        // Note that you must declare some width and height for the layout to work.

        CredentialsUI {
            id: credsUI

            width: parent.width * 0.80
            height: (parent.height - showCredsUISwitch.height) * 0.80
            anchors.horizontalCenter: parent.horizontalCenter 
            visible: showCredsUISwitch.checked
        }

        Label {
            id: infoLabel
            text: "Press this button to check for credentials in local keyring:"
            color: "white"
        }

        Button {
            id: btnCancel
            text: "Check existing creds"
            color: "#1c091a"
            onClicked: {
                console.debug("checking for creds in keyring:");
                u1credservice.checkCredentials();
            }
        }

    } // Column

    /* Below, an example of how to use the service without showing UI,
       just to test if there is a local credential already, and sign a
       url with it, if it does exist.*/

    UbuntuOneCredentialsService {
        id: u1credservice
        
        onCredentialsFound: {
            infoLabel.text = "Credentials Found. signing a url.";
            signUrl("http://server", "GET");
        }

        onUrlSigned: {
            infoLabel.text = "signed url is "+ signedUrl;
        }

        onUrlSigningError: {
            infoLabel.text = "signing error:" + errorMessage;
        }

        onCredentialsNotFound: {
            infoLabel.text = "No Credentials Found.";
            signUrl("http://server", "GET"); // xfail
        }
    }
} // UbuntuShape
