import QtQuick 2.0
import Ubuntu.Components 0.1

Row {
    id: buttons
    height: units.gu(5)
    spacing: units.gu(1)
    Button {
        id: btnCancel
        objectName: "cancelButton"
        text: "Cancel"
        color: "#1c091a"
        height: parent.height
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: {
            userCancelled();
        }
    }
    Button {
        id: btnContinue
        objectName: "continueButton"
        text: "Continue"
        color: "#cc3300"
        height: parent.height
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: {
            processForm();
        }
    }
}
