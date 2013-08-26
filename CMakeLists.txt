cmake_minimum_required (VERSION 2.8.9)

# This is the name and release version of the project
PROJECT (ubuntuone-credentials)
SET (PROJECT_VERSION 99.12)

# Some variables for the library builds
SET (AUTH_LIB_VERSION 0.1.0)
SET (AUTH_LIB_SOVERSION 0)
SET (AUTH_LIB_API_VERSION 2.0)
SET (AUTH_LIB_NAME ubuntuoneauth-${AUTH_LIB_API_VERSION})

SET (AUTHUI_LIB_VERSION 0.1.0)
SET (AUTHUI_LIB_SOVERSION 0)
SET (AUTHUI_LIB_API_VERSION 2.0)
SET (AUTHUI_LIB_NAME ubuntuoneauthui-${AUTHUI_LIB_API_VERSION})

# these names are also used in qml-credentials-service/qmldir, be sure to change them in both places
SET (QML_MODULE_NAME UbuntuOne)
SET (QML_PLUGIN_API_VERSION 1.0)
SET (QML_PLUGIN_NAME UbuntuOneCredentialsPlugin-${QML_PLUGIN_API_VERSION})

# For the signon plug-in module
SET (SIGNON_PLUGIN_NAME ubuntuoneplugin)

# Need to get libsignon/accounts here, as they get exposed in headers
find_package (PkgConfig REQUIRED)
pkg_check_modules(LIBSIGNON REQUIRED libsignon-qt5 accounts-qt5)
add_definitions(${LIBSIGNON_CFLAGS} ${LIBSIGNON_CFLAGS_OTHER})

# Some default CFLAGS
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -Werror -fPIC")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2 -g -Wall -Werror -fPIC")

# Allow specifying the libdir to use for the C library
SET (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name.")
SET (LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

set (QML_MODULE_INSTALL_DIR ${LIB_INSTALL_DIR}/qt5/qml )

include_directories(${CMAKE_SOURCE_DIR}/libubuntuoneauth
  ${CMAKE_SOURCE_DIR}/libubuntuoneauthui)

# A rule and variables for creating a tarball distribution
SET (ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
SET (ARCHIVE_FILE ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2)
SET (ARCHIVE_ARGS --exclude-vcs --exclude-backups --exclude-from=${CMAKE_SOURCE_DIR}/.bzrignore --transform=\"s,^,${ARCHIVE_NAME}/,\")
add_custom_target(dist
    COMMAND tar -cvjf ${ARCHIVE_FILE} ${ARCHIVE_ARGS} *
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


add_subdirectory(data)
add_subdirectory(libubuntuoneauth)
add_subdirectory(libubuntuoneauthui)
add_subdirectory(music-login)
add_subdirectory(signon-plugin)
add_subdirectory(qml-credentials-service)
add_subdirectory(online-accounts-provider)

# We need to add these here which simply depend on other test rules, as
# CMake doesn't allow defining a "check" rule in multiple places.
# We use $namespace-tests[-valgrind[-leaks]] for the test names in subdirs,
# and depend on them here, so that 'make check' will run them all.
add_custom_target(check
  DEPENDS ubuntuoneauth-tests ubuntuoneauthui-tests music-login-tests
)

add_custom_target(check-valgrind
  DEPENDS ubuntuoneauth-tests-valgrind ubuntuoneauthui-tests-valgrind music-login-tests-valgrind
)

add_custom_target(check-valgrind-leaks
  DEPENDS ubuntuoneauth-tests-valgrind-leaks ubuntuoneauthui-tests-valgrind-leaks music-login-tests-valgrind-leaks
)
