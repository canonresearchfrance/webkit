# - Try to find Gssdp 1.0
# This module defines the following variables:
#
#  GSSDP_FOUND - Gssdp 1.0 was found
#  GSSDP_INCLUDE_DIRS - the Gssdp 1.0 include directories
#  GSSDP_LIBRARIES - link these to use Gssdp 1.0
#

find_package(PkgConfig)
pkg_check_modules(PC_GSSDP REQUIRED QUIET gssdp-1.0)

find_path(GSSDP_INCLUDE_DIRS
    NAMES libgssdp/gssdp.h
    HINTS ${PC_GSSDP_INCLUDEDIR}
          ${PC_GSSDP_INCLUDE_DIRS}
    PATH_SUFFIXES libgssdp-1.0
)

find_library(GSSDP_LIBRARIES
    NAMES gssdp-1.0
    HINTS ${PC_GSSDP_LIBDIR}
          ${PC_GSSDP_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gssdp REQUIRED_VARS GSSDP_INCLUDE_DIRS GSSDP_LIBRARIES
                                          VERSION_VAR   PC_GSSDP_VERSION)
