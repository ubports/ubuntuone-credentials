SET (TESTS_TARGET test-online-accounts-provider)

set (AUTOPILOT_PACKAGE ubuntuone_credentials)

add_custom_target(online-accounts-provider-autopilot-tests
    COMMAND autopilot3 run ${AUTOPILOT_PACKAGE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/autopilot
    DEPENDS ${QML_PLUGIN_NAME}
)

add_custom_target(online-accounts-provider-autopilot-tests-local
    COMMAND SSO_AUTH_BASE_URL=fake SSO_UONE_BASE_URL=fake autopilot3 run ${AUTOPILOT_PACKAGE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/autopilot
    DEPENDS ${QML_PLUGIN_NAME}
)

add_custom_target(online-accounts-provider-qmlunit-tests
    COMMAND qmltestrunner -input ${CMAKE_CURRENT_SOURCE_DIR}/unit
)

set (Python_ADDITIONAL_VERSIONS 2.7)
find_package (PythonInterp)

set (AUTOPILOT_DIR autopilot/ubuntuone_credentials)

execute_process (
  COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib()"
  COMMAND sed -r -e "s|^/usr/(local/)?||g"
  OUTPUT_VARIABLE PYTHON_PACKAGE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

install (
  DIRECTORY ${AUTOPILOT_DIR}
  DESTINATION ${PYTHON_PACKAGE_DIR}
)
