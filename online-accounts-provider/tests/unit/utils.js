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

.pragma library

// Find an object with the given name in the children tree of "obj"
function findChild(obj,objectName) {
    var childs = new Array(0);
    childs.push(obj)
    while (childs.length > 0) {
        if (childs[0].objectName == objectName) {
            return childs[0]
        }
        for (var i in childs[0].children) {
            childs.push(childs[0].children[i])
        }
        childs.splice(0, 1);
    }
    return undefined;
}
