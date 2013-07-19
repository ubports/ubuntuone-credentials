# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# Allow the developer to select if Dynamic or Static libraries are built
OPTION (BUILD_STATIC_LIBS "Build Static Libraries" OFF)

# Set the LIB_TYPE variable to STATIC
SET (LIB_TYPE SHARED)
IF (BUILD_STATIC_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE 'STATIC'
  SET (LIB_TYPE STATIC)
ENDIF (BUILD_STATIC_LIBS)

# The sources for building the library
FILE (GLOB SOURCES *.cpp)
# HEADERS only includes the public headers for installation.
FILE (GLOB HEADERS *.h)

find_package (PkgConfig REQUIRED)
pkg_check_modules(LIBSECRET REQUIRED libsecret-1)
pkg_check_modules(OAUTH REQUIRED oauth)
add_definitions(${LIBSECRET_CFLAGS} ${LIBSECRET_CFLAGS_OTHER})
add_definitions(${OAUTH_CFLAGS} ${OAUTH_CFLAGS_OTHER})

# Need the project version here
add_definitions("-DPROJECT_VERSION=\"${PROJECT_VERSION}\"")

add_library (${AUTH_LIB_NAME} ${LIB_TYPE} ${SOURCES})
qt5_use_modules (${AUTH_LIB_NAME} Core DBus Xml Network)
target_link_libraries (${AUTH_LIB_NAME}
  ${LIBSECRET_LDFLAGS}
  ${OAUTH_LDFLAGS}
  -Wl,--version-script -Wl,libubuntuoneauth.symbols
)

SET_TARGET_PROPERTIES(${AUTH_LIB_NAME} PROPERTIES
  VERSION ${AUTH_LIB_VERSION}
  SOVERSION ${AUTH_LIB_SOVERSION}
)

INSTALL (
  TARGETS ${AUTH_LIB_NAME}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
)

INSTALL (
  FILES ${HEADERS}
  DESTINATION include/${AUTH_LIB_NAME}
)

add_subdirectory(tests)
