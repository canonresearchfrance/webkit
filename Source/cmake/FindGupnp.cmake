# - Try to find Gupnp 1.0
# This module defines the following variables:
#
#  GUPNP_FOUND - Gupnp 1.0 was found
#  GUPNP_INCLUDE_DIRS - the Gupnp 1.0 include directories
#  GUPNP_LIBRARIES - link these to use Gupnp 1.0
#

find_package(PkgConfig)
pkg_check_modules(PC_GUPNP REQUIRED QUIET gupnp-1.0)

find_path(GUPNP_INCLUDE_DIRS
    NAMES libgupnp/gupnp.h
    HINTS ${PC_GUPNP_INCLUDEDIR}
          ${PC_GUPNP_INCLUDE_DIRS}
    PATH_SUFFIXES libgupnp-1.0
)

find_library(GUPNP_LIBRARIES
    NAMES gupnp-1.0
    HINTS ${PC_GUPNP_LIBDIR}
          ${PC_GUPNP_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gupnp REQUIRED_VARS GUPNP_INCLUDE_DIRS GUPNP_LIBRARIES
                                          VERSION_VAR   PC_GUPNP_VERSION)
