# The thing we're building in here
SET (TESTS_TARGET test-ubuntuone-plugin)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

pkg_check_modules(SIGNON REQUIRED signon-plugins)
add_definitions(
    ${SIGNON_CFLAGS}
    ${SIGNON_CFLAGS_OTHER}
    "-DPLUGIN_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/../libubuntuoneplugin.so\""
)

# Workaround for cmake not adding these to automoc properly
SET (CMAKE_AUTOMOC_MOC_OPTIONS "${SIGNON_CFLAGS} ${CMAKE_AUTOMOC_MOC_OPTIONS}")

# The sources for building the tests
SET (SOURCES
    ${SignonPlugin_SOURCE_DIR}/i18n.cpp
    ${SignonPlugin_SOURCE_DIR}/ubuntuone-plugin.cpp
    test_plugin.cpp
)

include_directories( 
    ${SignonPlugin_SOURCE_DIR}
)

add_executable (${TESTS_TARGET} ${SOURCES})
qt5_use_modules (${TESTS_TARGET} Test Network)
target_link_libraries (${TESTS_TARGET}
    -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
    -L${CMAKE_BINARY_DIR}/libubuntuoneauth
    ${AUTH_LIB_NAME}
    ${SIGNON_LDFLAGS}
)

add_custom_target(ubuntuone-plugin-tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(ubuntuone-plugin-tests-valgrind
    COMMAND valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(ubuntuone-plugin-tests-valgrind-leaks
    COMMAND valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)
