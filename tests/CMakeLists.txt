# The thing we're building in here
SET (TESTS_TARGET test-ubuntuoneauth)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the tests
FILE (GLOB SOURCES sso_api/*.cpp main.cpp)

add_executable (${TESTS_TARGET} ${SOURCES})
qt5_use_modules (${TESTS_TARGET} Test)
target_link_libraries (${TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/lib
  -L${CMAKE_BINARY_DIR}/lib
  ${AUTH_LIB_NAME})

add_custom_target(check
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(check-valgrind
    COMMAND valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)

add_custom_target(check-valgrind-leaks
    COMMAND valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${TESTS_TARGET}
    DEPENDS ${TESTS_TARGET}
)
