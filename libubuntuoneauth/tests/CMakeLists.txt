# The thing we're building in here
SET (TESTS_TARGET test-ubuntuoneauth)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the tests
FILE (GLOB SOURCES *.cpp)

add_executable (${TESTS_TARGET} ${SOURCES})
qt5_use_modules (${TESTS_TARGET} Test DBus Network)
target_link_libraries (${TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME})

configure_file(
  mock_sso_server.py.in
  mock_sso_server.py
)

add_custom_target(ubuntuoneauth-tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(ubuntuoneauth-tests-valgrind
    COMMAND valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(ubuntuoneauth-tests-valgrind-leaks
    COMMAND valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)
