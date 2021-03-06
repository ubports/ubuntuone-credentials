project(SignonPlugin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined")

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the library
FILE (GLOB SOURCES *.cpp)
# HEADERS only includes the public headers for installation.
FILE (GLOB HEADERS *.h)

pkg_check_modules(SIGNON REQUIRED signon-plugins glib-2.0)
add_definitions(${SIGNON_CFLAGS} ${SIGNON_CFLAGS_OTHER})

# Workaround for cmake not adding these to automoc properly
SET (CMAKE_AUTOMOC_MOC_OPTIONS "${SIGNON_CFLAGS} ${CMAKE_AUTOMOC_MOC_OPTIONS}")

# Need the project version here
add_definitions(
  "-DPROJECT_VERSION=\"${PROJECT_VERSION}\""
  "-DGETTEXT_PACKAGE=\"ubuntuone-credentials\""
)

add_library (${SIGNON_PLUGIN_NAME} MODULE ${SOURCES})
qt5_use_modules (${SIGNON_PLUGIN_NAME} Core Xml Network)
target_link_libraries (${SIGNON_PLUGIN_NAME}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME}
  ${SIGNON_PLUGIN_LDFLAGS}
  ${SIGNON_LDFLAGS}
  ${SIGNON_LDFLAGS_OTHER}
)

SET (SIGNON_PLUGIN_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/signon)

INSTALL (
  TARGETS ${SIGNON_PLUGIN_NAME}
  LIBRARY DESTINATION ${SIGNON_PLUGIN_INSTALL_DIR}
)

add_subdirectory(tests)
