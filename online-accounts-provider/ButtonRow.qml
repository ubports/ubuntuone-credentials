import QtQuick 2.0
import Ubuntu.Components 0.1

Row {
    id: buttons
    height: units.gu(5)
    spacing: units.gu(1)
    Button {
        id: btnCancel
        text: "Cancel"
        color: "#1c091a"
        height: parent.height
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: {
            root.finished();
        }
    }
    Button {
        id: btnContinue
        text: "Continue"
        color: "#cc3300"
        height: parent.height
        width: (parent.width / 2) - 0.5 * parent.spacing
        onClicked: {
            processForm();
        }
    }
}
