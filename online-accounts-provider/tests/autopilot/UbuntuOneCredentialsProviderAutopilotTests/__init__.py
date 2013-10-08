# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.  

"""UbuntuOne Credentials Online-Accounts provider plugin autopilot tests."""

import os
import subprocess
import tempfile

from autopilot import input
from autopilot.matchers import Eventually
from testtools.matchers import Equals
from ubuntuuitoolkit import base, emulators


def _get_module_include_path():
    return _get_path_to_source_root()


def _get_path_to_source_root():
    return os.path.abspath(
        os.path.join(
            os.path.dirname(__file__), '..', '..', '..'))


def _get_local_desktop_file_directory():
    return os.path.join(os.environ['HOME'], '.local', 'share', 'applications')


def _get_qmlscene_launch_command():
    """Return the command to launch qmlscene for autopilot tests."""
    # TODO replace this with the one from the ubuntu-ui-toolkit, that has just
    # been merged to trunk. --elopio - 2013-10-08
    arch = subprocess.check_output(
        ["dpkg-architecture", "-qDEB_HOST_MULTIARCH"]).strip()
    return '/usr/lib/' + arch + '/qt5/bin/qmlscene'


class TestCaseWithQMLWrapper(base.UbuntuUIToolkitAppTestCase):

    _DESKTOP_FILE_CONTENTS = ("""[Desktop Entry]
Type=Application
Exec=Not important
Path=Not important
Name=Test app
Icon=Not important
""")
    
    test_qml_wrapper_file_name = None

    def setUp(self):
        super(TestCaseWithQMLWrapper, self).setUp()
        self.pointing_device = input.Pointer(self.input_device_class.create())
        self.keyboard_ = input.Keyboard.create()
        self.launch_application()

    def launch_application(self):
        qml_file_path = os.path.join(
            os.path.dirname(__file__), self.test_qml_wrapper_file_name)
        desktop_file_path = self._write_test_desktop_file()
        self.addCleanup(os.remove, desktop_file_path)
        self.app = self.launch_test_application(
            _get_qmlscene_launch_command(),
            '-I' + _get_module_include_path(),
            qml_file_path,
            '--desktop_file_hint={0}'.format(desktop_file_path),
        emulator_base=emulators.UbuntuUIToolkitEmulatorBase,
        app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _write_test_desktop_file(self):
        desktop_file_dir = _get_local_desktop_file_directory()
        if not os.path.exists(desktop_file_dir):
            os.makedirs(desktop_file_dir)
        desktop_file = tempfile.NamedTemporaryFile(
            suffix='.desktop', dir=desktop_file_dir, delete=False)
        desktop_file.write(self._DESKTOP_FILE_CONTENTS)
        desktop_file.close()
        return desktop_file.name

    @property
    def main_view(self):
        return self.app.select_single(emulators.MainView)
