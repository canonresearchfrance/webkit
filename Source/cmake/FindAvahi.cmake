# - Try to find Avahi
# This module defines the following variables:
#
#  AVAHI_FOUND - Avahi was found
#  AVAHI_INCLUDE_DIRS - the Avahi include directories
#  AVAHI_LIBRARIES - link these to use Avahi
#  AVAHI_GLIB_LIBRARIES - link these to use Avahi Glib
#

find_package(PkgConfig)
pkg_check_modules(PC_AVAHI_CLIENT REQUIRED QUIET avahi-client)
pkg_check_modules(PC_AVAHI_GLIB REQUIRED QUIET avahi-glib)

find_path(AVAHI_COMMON_INCLUDE_DIRS
    NAMES error.h malloc.h domain.h llist.h
    HINTS ${PC_AVAHI_COMMON_INCLUDEDIR}
          ${PC_AVAHI_COMMON_INCLUDE_DIRS}
    PATH_SUFFIXES avahi-common
)

find_path(AVAHI_CLIENT_INCLUDE_DIRS
    NAMES client.h lookup.h
    HINTS ${PC_AVAHI_CLIENT_INCLUDEDIR}
          ${PC_AVAHI_CLIENT_INCLUDE_DIRS}
    PATH_SUFFIXES avahi-client
)

find_path(AVAHI_GLIB_INCLUDE_DIRS
    NAMES glib-malloc.h glib-watch.h
    HINTS ${PC_AVAHI_GLIB_INCLUDEDIR}
          ${PC_AVAHI_GLIB_INCLUDE_DIRS}
    PATH_SUFFIXES avahi-glib
)

find_library(AVAHI_COMMON_LIBRARIES
    NAMES avahi-common
    HINTS ${PC_AVAHI_COMMON_LIBDIR}
          ${PC_AVAHI_COMMON_LIBRARY_DIRS}
)

find_library(AVAHI_CLIENT_LIBRARIES
    NAMES avahi-client
    HINTS ${PC_AVAHI_LIBDIR}
          ${PC_AVAHI_LIBRARY_DIRS}
)

find_library(AVAHI_GLIB_LIBRARIES
    NAMES avahi-glib
    HINTS ${PC_AVAHI_GLIB_LIBDIR}
          ${PC_AVAHI_GLIB_LIBRARY_DIRS}
)

set(AVAHI_INCLUDE_DIRS ${AVAHI_CLIENT_INCLUDE_DIRS} ${AVAHI_GLIB_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Avahi REQUIRED_VARS AVAHI_INCLUDE_DIRS AVAHI_COMMON_LIBRARIES AVAHI_CLIENT_LIBRARIES AVAHI_GLIB_LIBRARIES
                                          VERSION_VAR   PC_AVAHI_VERSION)
