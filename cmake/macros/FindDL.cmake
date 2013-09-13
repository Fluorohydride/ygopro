# - Try to find the dl library
# Once done this will define
#
#  DL_FOUND - System has libdl
#  DL_LIBRARIES - The libraries needed to use libdl
#  DL_DEFINITIONS - Compiler switches required for using libdl

FIND_PATH(DL_INCLUDE_DIR dlfcn.h
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(DL_LIBRARIES NAMES dl
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libdl DEFAULT_MSG DL_LIBRARIES DL_INCLUDE_DIR)

MARK_AS_ADVANCED(DL_INCLUDE_DIR DL_LIBRARIES)
