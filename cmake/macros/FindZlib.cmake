# - Try to find the Lib library
# Once done this will define
#
#  ZLIB_FOUND - System has zlib
#  ZLIB_INCLUDE_DIR - The zlib include directory
#  ZLIB_LIBRARIES - The libraries needed to use zlib
#  ZLIB_DEFINITIONS - Compiler switches required for using zlib

SET(ZLIB_DEFINITIONS ${PC_ZLIB_CFLAGS_OTHER})

FIND_PATH(ZLIB_INCLUDE_DIR zlib.h
   HINTS
   ${PC_ZLIB_INCLUDEDIR}
   ${PC_ZLIB_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(ZLIB_LIBRARIES NAMES zlib
   HINTS
   ${PC_ZLIB_LIBDIR}
   ${PC_ZLIB_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(ZLIB_LIBRARIES ${ZLIB_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZLIB DEFAULT_MSG ZLIB_LIBRARIES ZLIB_INCLUDE_DIR)

MARK_AS_ADVANCED(ZLIB_INCLUDE_DIR ZLIB_LIBRARIES)
