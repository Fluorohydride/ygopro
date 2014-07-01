# - Try to find the Lib library
# Once done this will define
#
#  PCRE_FOUND - System has pcre
#  PCRE_INCLUDE_DIR - The pcre include directory
#  PCRE_LIBRARIES - The libraries needed to use pcre
#  PCRE_DEFINITIONS - Compiler switches required for using pcre

SET(PCRE_DEFINITIONS ${PC_PCRE_CFLAGS_OTHER})

FIND_PATH(PCRE_INCLUDE_DIR pcre.h
   HINTS
   ${PC_PCRE_INCLUDEDIR}
   ${PC_PCRE_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(PCRE_LIBRARIES NAMES pcre
   HINTS
   ${PC_PCRE_LIBDIR}
   ${PC_PCRE_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(PCRE_LIBRARIES ${PCRE_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE DEFAULT_MSG PCRE_LIBRARIES PCRE_INCLUDE_DIR)

MARK_AS_ADVANCED(PCRE_INCLUDE_DIR PCRE_LIBRARIES)
