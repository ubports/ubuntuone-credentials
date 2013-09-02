import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0

Rectangle {
    id: exampleMain
    width: units.gu(100)
    height: units.gu(120)
    color: UbuntuColors.coolGrey

    Column {
        spacing: units.gu(2)
        anchors.fill: parent

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
                u1credservice.checkCredentials();
            }
        }

        Label {
            id: revokeLabel
            text: "Press this button to delete the credentials in local keyring:"
            color: "white"
        }

        Button {
            id: btnRevoke
            text: "Invalidate existing creds"
            color: "#1c091a"
            onClicked: {
                u1credservice.invalidateCredentials();
            }
        }

    } // Column

    /* Here is the component that exposes the credentials API.
       You can use it to test if there is a local credential already, and sign a
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
            /* We could just report a generic error here, but to show what happens
               if you don't bother checking before you sign a URL, we
               sign a URL here, expecting it to fail. We'll then get
               onUrlSigningError(), with an error message saying
               something like "attempt to sign with no credentials in
               keyring". Note that "Keyring" is vestigial, we are not
               using the system keyring. */

            signUrl("http://server", "GET");
        }
        
        onCredentialsDeleted: {
            revokeLabel.text = "Deleted Credentials successfully";
        }
    }
} // UbuntuShape
