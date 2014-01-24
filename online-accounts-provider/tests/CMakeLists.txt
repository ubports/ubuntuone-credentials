SET (TESTS_TARGET test-online-accounts-provider)

set (AUTOPILOT_PACKAGE ubuntuone_credentials)

add_custom_target(online-accounts-provider-autopilot-tests
    COMMAND autopilot run ${AUTOPILOT_PACKAGE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/autopilot
    DEPENDS ${QML_PLUGIN_NAME}
)

add_custom_target(online-accounts-provider-autopilot-tests-local
    COMMAND SSO_AUTH_BASE_URL=fake SSO_UONE_BASE_URL=fake autopilot run ${AUTOPILOT_PACKAGE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/autopilot
    DEPENDS ${QML_PLUGIN_NAME}
)

add_custom_target(online-accounts-provider-qmlunit-tests
    COMMAND qmltestrunner -input ${CMAKE_CURRENT_SOURCE_DIR}/unit
)
