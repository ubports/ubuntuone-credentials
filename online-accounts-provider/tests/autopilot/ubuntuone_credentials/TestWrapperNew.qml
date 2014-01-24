import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.OnlineAccounts 0.1
import UbuntuOne 1.0


MainView {

    objectName: "mainView"

    applicationName: "Ubuntu One UOA Provider Plugin Tester"

    width: units.gu(100)    
    height: units.gu(100)

    PageStack {
        id: pageStack
        Component.onCompleted: {
            push(dummyPage);
            push(testingPage);
        }
    }
    
    Page {
        id: dummyPage
        objectName: "dummyPage"
        title: i18n.tr("Root page")
        visible: false
        /* property var success: false */

        Rectangle {
            anchors.fill: parent
            color: "green"
        }
        
    }

    Page {
        title: "testing"
        id: testingPage
        objectName: "testingPage"
        visible: false

        Rectangle {
            id: fakeAccount
            objectName: "FakeNewUOAAccount"
            property var accountId: 0
            signal synced

            property string displayName: "" 
            function sync() {
                /* synced() signal is connected to finished on success, so we
                  need to just forward it here */
                synced();
            }

            function updateDisplayName() {
            }
            
            function updateEnabled() {
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

            source: "../../../Main.qml"

            Connections {
                target: loader.item
                onFinished: {
                    /* dummyPage.success = true; */
                    pageStack.pop();
                }
            }

        }

    }
}