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
import Ubuntu.Components.ListItems 0.1 as ListItem
import Ubuntu.Components.Popups 0.1
import Ubuntu.OnlineAccounts 0.1
import Ubuntu.OnlineAccounts.Plugin 1.0

Column {
    id: root

    property variant __account: account

    signal finished

    anchors.left: parent.left
    anchors.right: parent.right

    ListItem.SingleValue {
        text: "User Name"
        value: account.displayName
    }

    ListItem.SingleValue {
        text: "Token Name"
        value: account.displayName
    }

    ServiceSwitches {
        account: __account
        enabled: __account.enabled
        opacity: enabled ? 1 : 0.5
    }

    // TOFIX: use ListItem.SingleControl when lp #1194844 is fixed
    ListItem.Base {
        Button {
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                left: parent.left
            }
            text: "Remove account…"
            onClicked: PopupUtils.open(removalConfirmationComponent); // huh? , removeBtn)
        }
        showDivider: false
    }

   Component {
        id: removalConfirmationComponent  
        Dialog {
            id: dialog

            property bool confirmed: false

            title: "Remove Credentials"
            text: "Removing the Ubuntu One credentials will prevent any applications on this device from accessing your account. You can authorize the device again by providing your username and password."

            Button {
                text: "Remove"
                onClicked: {
                    PopupUtils.close(dialog);
                    console.log("Removing account: " + __account);
                    account.removed.connect(root.finished);
                    account.remove(Account.RemoveCredentials);
                }

            }

            Button {
                text: "Cancel"
                onClicked: {
                    console.log("User cancelled account removal");
                    PopupUtils.close(dialog);
                }
            }
        } // Dialog
   } //Component
}
