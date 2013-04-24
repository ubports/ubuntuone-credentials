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
FILE (GLOB HEADERS *.h)

add_library (${AUTHUI_LIB_NAME} ${LIB_TYPE} ${SOURCES} ${HEADERS})
qt5_use_modules (${AUTHUI_LIB_NAME} Core DBus Xml Network)
target_link_libraries (${AUTHUI_LIB_NAME}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME})

SET_TARGET_PROPERTIES(${AUTHUI_LIB_NAME} PROPERTIES
  VERSION ${AUTHUI_LIB_VERSION}
  SOVERSION ${AUTHUI_LIB_SOVERSION}
)

INSTALL (
  TARGETS ${AUTHUI_LIB_NAME}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
)

INSTALL (
  FILES ${HEADERS}
  DESTINATION include/${AUTH_LIB_NAME}
)

add_subdirectory(tests)
