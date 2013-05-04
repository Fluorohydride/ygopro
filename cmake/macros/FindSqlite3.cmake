# - Try to find the Lib library
# Once done this will define
#
#  SQLITE3_FOUND - System has SQLITE3
#  SQLITE3_INCLUDE_DIR - The SQLITE3 include directory
#  SQLITE3_LIBRARIES - The libraries needed to use SQLITE3
#  SQLITE3_DEFINITIONS - Compiler switches required for using SQLITE3


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#FIND_PACKAGE(PkgConfig)
#PKG_SEARCH_MODULE(PC_SQLITE3 sqlite3)

SET(SQLITE3_DEFINITIONS ${PC_SQLITE3_CFLAGS_OTHER})

FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h
   HINTS
   ${PC_SQLITE3_INCLUDEDIR}
   ${PC_SQLITE3_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(SQLITE3_LIBRARIES NAMES sqlite3
   HINTS
   ${PC_SQLITE3_LIBDIR}
   ${PC_SQLITE3_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sqlite3 DEFAULT_MSG SQLITE3_LIBRARIES SQLITE3_INCLUDE_DIR)

MARK_AS_ADVANCED(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARIES)
