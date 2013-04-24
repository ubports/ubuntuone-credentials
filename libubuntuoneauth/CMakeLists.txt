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
FILE (GLOB SOURCES keyring/*.cpp dbus/*.cpp sso_api/*.cpp main/*.cpp ssoservice.cpp)
FILE (GLOB HEADERS keyring/*.h dbus/*.h sso_api/*.h main/*.h ssoservice.h)

add_library (${AUTH_LIB_NAME} ${LIB_TYPE} ${SOURCES} ${HEADERS})
qt5_use_modules (${AUTH_LIB_NAME} Core DBus Xml Network)

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
