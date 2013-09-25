import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.OnlineAccounts 0.1
import UbuntuOne 1.0


MainView {

    objectName: "mainView"

    applicationName: "Ubuntu One UOA Provider Plugin Tester"

    width: units.gu(100)    
    height: units.gu(100)

    Page {
        title: "testing"
        id: testingPage
        objectName: "testingPage"

        Rectangle {
            id: fakeAccount
            objectName: "FakeNewUOAAccount"
            property var accountId: 0
            signal synced

            property string displayName: "" 
            function sync() {
            }

            /* fake to avoid creating a UOA account */
            property var objectHandle: Qt.createQmlObject('import QtQuick 2.0; Rectangle {}',
                                                         testingPage, 
                                                         "dynamically-created-fakerect");
        }

        Loader {
            id: loader
            property var account: fakeAccount

            anchors.fill: parent

            source: "../../Main.qml"

        }

    }
}