SET (EXAMPLES_TARGET ubuntuoneauth-examples)

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

FILE (GLOB SIGNING_SOURCES signing-main.cpp)

add_executable (${SIGNING_TARGET} ${SIGNING_SOURCES})
qt5_use_modules (${SIGNING_TARGET} DBus Network)

target_link_libraries (${TESTS_TARGET}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME})
