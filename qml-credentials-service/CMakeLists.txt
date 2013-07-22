# Qt5 bits
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

file (GLOB CPP_SOURCES *.cpp)
file (GLOB QML_SOURCES *.qml)

configure_file (qmldir.template qmldir)

add_library (${PLUGIN_NAME} MODULE ${CPP_SOURCES})
qt5_use_modules (${PLUGIN_NAME} Core Widgets Quick Qml )
target_link_libraries (${PLUGIN_NAME}
  -Wl,-rpath,${CMAKE_BINARY_DIR}/libubuntuoneauth
  -L${CMAKE_BINARY_DIR}/libubuntuoneauth
  ${AUTH_LIB_NAME}
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir DESTINATION ${QML_MODULE_INSTALL_DIR})

install (TARGETS ${PLUGIN_NAME} 
  LIBRARY DESTINATION ${QML_MODULE_INSTALL_DIR}
  NAMELINK_SKIP
)

install (FILES ${QML_SOURCES} DESTINATION ${QML_MODULE_INSTALL_DIR})
