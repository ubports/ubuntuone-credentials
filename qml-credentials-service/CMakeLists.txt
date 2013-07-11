# Qt5 bits
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

file (GLOB CPP_SOURCES *.cpp)
file (GLOB QML_SOURCES *.qml)

add_library (${PLUGIN_NAME} MODULE ${CPP_SOURCES})
qt5_use_modules (${PLUGIN_NAME} Core Widgets Quick Qml )

configure_file (qmldir.template ${QML_MODULE_NAME}/qmldir)

add_custom_command (TARGET ${PLUGIN_NAME} POST_BUILD
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}.so ${QML_MODULE_NAME} 
  #TODO -- qmltypes
  COMMENT Building QML Component Directory in ${QML_MODULE_NAME} )

file (COPY ${QML_SOURCES} DESTINATION ${QML_MODULE_NAME} )

target_link_libraries (${PLUGIN_NAME} 
  ${AUTH_LIB_NAME}
)


install (DIRECTORY ${QML_MODULE_NAME} DESTINATION ${QML_MODULE_INSTALL_DIR})

#add_subdirectory(tests)
