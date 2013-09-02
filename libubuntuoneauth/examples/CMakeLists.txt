SET (EXAMPLES_TARGET ubuntuoneauth-examples)

SET (SIGNING_EXE "signing-example")

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

FILE (GLOB SIGNING_SOURCES signing*.cpp)
FILE (GLOB SIGNING_HEADERS signing*.h)

add_executable (${SIGNING_EXE}
  ${SIGNING_SOURCES}
  ${SIGNING_HEADERS})
qt5_use_modules (${SIGNING_EXE} DBus Network)

target_link_libraries (${SIGNING_EXE}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME})

add_custom_target(examples-valgrind
    COMMAND "valgrind --tool=memcheck ${CMAKE_CURRENT_BINARY_DIR}/${SIGNING_EXE}"
    DEPENDS ${SIGNING_EXE}
)

add_custom_target(examples-valgrind-leaks
    COMMAND "valgrind --tool=memcheck --track-origins=yes --num-callers=40 --leak-resolution=high --leak-check=full ${CMAKE_CURRENT_BINARY_DIR}/${SIGNING_EXE}"
    DEPENDS ${SIGNING_EXE}
)
