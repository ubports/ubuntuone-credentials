# The thing we're building in here
SET (SIGNON_TESTS_TARGET test-signon-plugin)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the tests
FILE (GLOB SOURCES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/signon-plugin)

find_package (PkgConfig REQUIRED)
pkg_check_modules(TESTSIGNON REQUIRED signon-plugins)
add_definitions(${TESTSIGNON_CFLAGS} ${TESTSIGNON_CFLAGS_OTHER})

add_executable (${SIGNON_TESTS_TARGET} ${SOURCES})
qt5_use_modules (${SIGNON_TESTS_TARGET} Test Xml Network)
target_link_libraries (${SIGNON_TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME}
  ${TESTSIGNON_LDFLAGS}
)

add_custom_target(signon-plugin-tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${SIGNON_TESTS_TARGET}
    DEPENDS ${SIGNON_TESTS_TARGET}
)

add_custom_target(signon-plugin-tests-valgrind
    COMMAND valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${SIGNON_TESTS_TARGET}
    DEPENDS ${SIGNON_TESTS_TARGET}
)

add_custom_target(signon-plugin-tests-valgrind-leaks
    COMMAND valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${SIGNON_TESTS_TARGET}
    DEPENDS ${SIGNON_TESTS_TARGET}
)
