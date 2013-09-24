file (GLOB QML_PLUGIN_FILES *.qml)

# The path (including plug-in name) where the QML files are installed
set (QML_PLUGIN_INSTALL_DIR share/accounts/qml-plugins/ubuntuone/)

install (FILES ${QML_PLUGIN_FILES} DESTINATION ${QML_PLUGIN_INSTALL_DIR})

add_subdirectory(tests)