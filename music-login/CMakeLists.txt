SET (MUSIC_LOGIN_EXE "ubuntuone-music-login")
SET (MUSIC_LOGIN_LIB "u1mslogin")

# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

# The sources for building the app
FILE (GLOB MAIN_SOURCES
  main.cpp
)

FILE (GLOB LIB_SOURCES
  header.cpp
  footer.cpp
  loadingoverlay.cpp
  loginform.cpp
  mainwindow.cpp
  registerform.cpp
  ssowizard.cpp
)

FILE (GLOB HEADERS *.h)
FILE (GLOB FORMS *.ui)
FILE (GLOB RESOURCES *.qrc)

qt5_add_resources (BUILT_RESOURCES ${RESOURCES})
qt5_wrap_ui (BUILT_FORMS ${FORMS})

add_library (${MUSIC_LOGIN_LIB} STATIC
  ${LIB_SOURCES}
  ${HEADERS}
  ${BUILT_FORMS}
)

add_executable (${MUSIC_LOGIN_EXE}
  ${MAIN_SOURCES}
  ${LIB_SOURCES}
  ${HEADERS}
  ${BUILT_FORMS}
  ${BUILT_RESOURCES}
)

qt5_use_modules (${MUSIC_LOGIN_LIB} Core DBus Gui Widgets)
qt5_use_modules (${MUSIC_LOGIN_EXE} DBus Widgets)
target_link_libraries (${MUSIC_LOGIN_EXE}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/lib
  -L${CMAKE_BINARY_DIR}/lib
  ${AUTH_LIB_NAME}
  ${MUSIC_LIB_NAME}
)

INSTALL (
  TARGETS ${MUSIC_LOGIN_EXE}
  RUNTIME DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}
)

add_subdirectory(tests)
