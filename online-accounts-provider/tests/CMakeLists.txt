SET (TESTS_TARGET test-online-accounts-provider)

add_custom_target(online-accounts-provider-autopilot-tests
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/autopilot/run ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QML_PLUGIN_NAME}
)

add_custom_target(online-accounts-provider-qmlunit-tests
    COMMAND qmltestrunner -input ${CMAKE_CURRENT_SOURCE_DIR}/unit
)
