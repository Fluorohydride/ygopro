# - Try to find the Lib library
# Once done this will define
#
#  SQLITE_FOUND - System has sqlite
#  SQLITE_INCLUDE_DIR - The sqlite include directory
#  SQLITE_LIBRARIES - The libraries needed to use sqlite
#  SQLITE_DEFINITIONS - Compiler switches required for using sqlite

SET(SQLITE_DEFINITIONS ${PC_SQLITE_CFLAGS_OTHER})

FIND_PATH(SQLITE_INCLUDE_DIR sqlite3.h
   HINTS
   ${PC_SQLITE_INCLUDEDIR}
   ${PC_SQLITE_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(SQLITE_LIBRARIES NAMES sqlite3
   HINTS
   ${PC_SQLITE_LIBDIR}
   ${PC_SQLITE_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(SQLITE_LIBRARIES ${SQLITE_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE DEFAULT_MSG SQLITE_LIBRARIES SQLITE_INCLUDE_DIR)

MARK_AS_ADVANCED(SQLITE_INCLUDE_DIR SQLITE_LIBRARIES)
