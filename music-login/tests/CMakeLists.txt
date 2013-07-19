# The thing we're building in here
SET (UI_TESTS_TARGET test-music-login)

# This is needed for the tests to build due to private members
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTESTS")

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

# The sources for building the tests
FILE (GLOB SOURCES main.cpp *.cpp)
FILE (GLOB HEADERS *.h)

include_directories(${CMAKE_SOURCE_DIR}/music-login
  ${CMAKE_BINARY_DIR}/music-login)

add_executable (${UI_TESTS_TARGET} ${SOURCES} ${HEADERS})
qt5_use_modules (${UI_TESTS_TARGET} Test DBus Network Widgets)
add_dependencies (${UI_TESTS_TARGET} ${MUSIC_LOGIN_LIB})
target_link_libraries (${UI_TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME}
  ${MUSIC_LOGIN_LIB}
)

add_custom_target(music-login-tests
    COMMAND xvfb-run -a "${CMAKE_CURRENT_BINARY_DIR}/${UI_TESTS_TARGET}"
    DEPENDS ${UI_TESTS_TARGET}
)

add_custom_target(music-login-tests-valgrind
    COMMAND xvfb-run -a "valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${UI_TESTS_TARGET}"
    DEPENDS ${UI_TESTS_TARGET}
)

add_custom_target(music-login-tests-valgrind-leaks
    COMMAND xvfb-run -a "valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${UI_TESTS_TARGET}"
    DEPENDS ${UI_TESTS_TARGET}
)
