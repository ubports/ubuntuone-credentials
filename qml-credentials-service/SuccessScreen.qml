import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "transparent"

    Rectangle{
        anchors.fill: parent
        anchors.leftMargin: units.gu(4)
        anchors.topMargin: units.gu(20)
        Label {
            font.bold: true
            fontSize: "x-large"
            width: parent.width
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: "#dd4814"
            text: "LOGIN SUCCESSFUL!"
        }
    }
}
