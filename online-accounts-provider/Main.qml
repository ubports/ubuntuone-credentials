/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.OnlineAccounts 0.1

Flickable {
    id: rootFlickable

    property int keyboardSize: Qt.inputMethod.visible ? Qt.inputMethod.keyboardRectangle.height : 0
    contentHeight: editPageLoader.item.height + keyboardSize

    property url newAccountUrl: "NewAccount.qml"
    property url existingAccountUrl: "ExistingAccount.qml"
    property Component newAccountComponent: null
    property Component existingAccountComponent: null

    signal finished

    Loader {
        id: editPageLoader
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        source: sourceComponent === null ? (account.accountId != 0 ? existingAccountUrl : newAccountUrl) : ""
        sourceComponent: account.accountId != 0 ? existingAccountComponent : newAccountComponent

        onLoaded: {
            /* NOTE: dirty hack to work around out grandparent Page (from
               AccountCreationPage.qml in
               ubuntu-system-settings-online-accounts) not being able
               to find our Flickable, and not having an id set for us
               to set flickable directly.

               This will not be necessary once Bug #1221845 is fixed.
             */
            rootFlickable.parent.parent.flickable = rootFlickable;
        }

        Connections {
            target: rootFlickable.parent.parent.tools.back;
            onTriggered: {
                console.debug("page back button clicked, deleting incomplete account.");
                if(account.accountId != 0) {
                    account.remove(Account.RemoveCredentials);
                    editPageLoader.item.resetUI();
                }
            }
        }

        Connections {
            target: account
            onSynced: {
                console.debug("Account id " + account.accountId + " synced. account.displayName: " + account.displayName)
            }
        }
        
        Connections {
            target: editPageLoader.item
            onFinished: rootFlickable.finished()
        }
    }

    Rectangle {
        id: loadingOverlay
        objectName: "loadingOverlay"
        opacity: 0.7
        color: "white"
        visible: false
        anchors.fill: parent

        ActivityIndicator {
            id: activity
            anchors.centerIn: parent
            running: parent.visible
        }
    }

}
