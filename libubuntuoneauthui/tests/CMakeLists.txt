# The thing we're building in here
SET (AUTHUI_TESTS_TARGET test-ubuntuoneauthui)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the tests
FILE (GLOB SOURCES main.cpp)

add_executable (${AUTHUI_TESTS_TARGET} ${SOURCES})
qt5_use_modules (${AUTHUI_TESTS_TARGET} Test)
target_link_libraries (${AUTHUI_TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauthui
  -L${CMAKE_BINARY_DIR}/libubuntuoneauthui
  ${AUTHUI_LIB_NAME})

add_custom_target(ubuntuoneauthui-tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${AUTHUI_TESTS_TARGET}
    DEPENDS ${AUTHUI_TESTS_TARGET}
)

add_custom_target(ubuntuoneauthui-tests-valgrind
    COMMAND valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${AUTHUI_TESTS_TARGET}
    DEPENDS ${AUTHUI_TESTS_TARGET}
)

add_custom_target(ubuntuoneauthui-tests-valgrind-leaks
    COMMAND valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${AUTHUI_TESTS_TARGET}
    DEPENDS ${AUTHUI_TESTS_TARGET}
)
