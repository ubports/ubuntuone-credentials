# Qt5 bits
SET (CMAKE_INCLUDE_CURRENT_DIR ON)
SET (CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

# The sources for building the library
FILE (GLOB SOURCES *.cpp)
# HEADERS only includes the public headers for installation.
FILE (GLOB HEADERS *.h)

SET (ACL_UPDATER acl-updater)

add_executable (${ACL_UPDATER} ${SOURCES})
qt5_use_modules (${ACL_UPDATER} Core)
target_link_libraries (${ACL_UPDATER}
    ${LIBSIGNON_LDFLAGS}
)

# We don't install this; dh-migrations will pick it up from the build dir.
