cmake_minimum_required (VERSION 2.8.9)

# This is the name and release version of the project
PROJECT (ubuntuone-credentials)
SET (PROJECT_VERSION 4.1.92)

# Some variables for the library builds
SET (AUTH_LIB_VERSION 0.1.0)
SET (AUTH_LIB_SOVERSION 0)
SET (AUTH_LIB_API_VERSION 2.0)
SET (AUTH_LIB_NAME ubuntuoneauth-${AUTH_LIB_API_VERSION})

SET (AUTHUI_LIB_VERSION 0.1.0)
SET (AUTHUI_LIB_SOVERSION 0)
SET (AUTHUI_LIB_API_VERSION 2.0)
SET (AUTHUI_LIB_NAME ubuntuoneauthui-${AUTHUI_LIB_API_VERSION})

# Some default CFLAGS
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -Werror -fPIC")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2 -g -Wall -Werror -fPIC")

# Allow specifying the libdir to use for the C library
SET (LIB_SUFFIX "" CACHE STRING "Define suffix of library directory name.")
SET (LIB_INSTALL_DIR "lib${LIB_SUFFIX}")

include_directories(${CMAKE_SOURCE_DIR}/libubuntuoneauth
  ${CMAKE_SOURCE_DIR}/libubuntuoneauthui)

# A rule and variables for creating a tarball distribution
SET (ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
SET (ARCHIVE_FILE ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2)
SET (ARCHIVE_ARGS --exclude-vcs --exclude-backups --exclude-from=${CMAKE_SOURCE_DIR}/.bzrignore --transform=\"s,^,${ARCHIVE_NAME}/,\")
add_custom_target(dist
    COMMAND tar -cvjf ${ARCHIVE_FILE} ${ARCHIVE_ARGS} *
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


add_subdirectory(libubuntuoneauth)
add_subdirectory(libubuntuoneauthui)
add_subdirectory(music-login)

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