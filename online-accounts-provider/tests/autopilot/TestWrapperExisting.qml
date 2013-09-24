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
        objectName: "testingPage"

        Rectangle {
            id: fakeAccount
            objectName: "FakeExistingUOAAccount"
            property var accountId: 47
            signal synced
            
            property string displayName: "my@email.com"
            function remove(){
            }
        }

        Loader {
            id: loader
            property var account: fakeAccount

            anchors.fill: parent

            source: "../../Main.qml"

        }

    }
}