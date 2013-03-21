cmake_minimum_required (VERSION 2.8.9)

# This is the name and release version of the project
PROJECT (ubuntuone-credentials)
SET (PROJECT_VERSION 4.1.92)

# Some variables for the library build
SET (AUTH_LIB_VERSION 0.1.0)
SET (AUTH_LIB_SOVERSION 0)
SET (AUTH_LIB_API_VERSION 2.0)
SET (AUTH_LIB_NAME ubuntuoneauth-${AUTH_LIB_API_VERSION})

# Some default CFLAGS
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall -Werror -fPIC")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O2 -g -Wall -Werror -fPIC")

include_directories(${CMAKE_SOURCE_DIR}/lib)

# A rule and variables for creating a tarball distribution
SET (ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
SET (ARCHIVE_FILE ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2)
SET (ARCHIVE_ARGS --exclude-vcs --exclude-backups --exclude-from=${CMAKE_SOURCE_DIR}/.bzrignore --transform=\"s,^,${ARCHIVE_NAME}/,\")
add_custom_target(dist
    COMMAND tar -cvjf ${ARCHIVE_FILE} ${ARCHIVE_ARGS} *
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


add_subdirectory(lib)
add_subdirectory(tests)
