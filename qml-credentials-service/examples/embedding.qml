import QtQuick 2.0
import Ubuntu.Components 0.1
import UbuntuOne 1.0

Rectangle {
        id: main
        width: 800
        height: 600
        color: "white"

        Button {
        id: showcredsUIButton
        text: "Show Creds UI"
        onClicked: {
                CredentialsUI.main