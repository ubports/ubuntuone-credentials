/*
 * Copyright (C) 2013-2014 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.Components.Popups 1.3
import Ubuntu.OnlineAccounts 0.1
import Ubuntu.OnlineAccounts.Plugin 1.0

Column {
    id: root

    property variant __account: account

    signal finished

    anchors.left: parent.left
    anchors.right: parent.right

    ListItem.SingleValue {
        objectName: "emailListItem"
        text: i18n.dtr(rootFlickable.domain, "Email")
        value: account.displayName
    }

    ServiceSwitches {
        account: __account
        enabled: __account.enabled
        opacity: enabled ? 1 : 0.5
    }

    ListItem.SingleControl {
        control: Button {
            objectName: "removeAccountButton"
            text: i18n.dtr(rootFlickable.domain, "Remove account…")
            width: parent.width - units.gu(4)
            onClicked: PopupUtils.open(removalConfirmationComponent);
        }
        showDivider: false
    }

    Component {
        id: removalConfirmationComponent  
        Dialog {
            id: dialog

            property bool confirmed: false

            title: i18n.dtr(rootFlickable.domain, "Remove Credentials")
            text: i18n.dtr(rootFlickable.domain, "Removing the Ubuntu One credentials will prevent any applications on this device from accessing your account. You can authorize the device again by providing your username and password.")

            Button {
                text: i18n.dtr(rootFlickable.domain, "Remove")
                objectName: "confirmRemoveAccountButton"
                onClicked: {
                    PopupUtils.close(dialog);
                    console.debug("Removing account ID: " + account.accountId);
                    account.removed.connect(root.finished);
                    account.remove(Account.RemoveCredentials);
                    console.debug("removed account ID: " + account.accountId);
                }

            }

            Button {
                text: i18n.dtr(rootFlickable.domain, "Cancel")
                objectName: "cancelRemoveAccountButton"
                onClicked: {
                    console.log("User cancelled account removal");
                    PopupUtils.close(dialog);
                }
            }
        } // Dialog
    } //Component
}
