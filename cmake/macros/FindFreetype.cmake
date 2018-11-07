# - Try to find the Lib library
# Once done this will define
#
#  FREETYPE_FOUND - System has freetype
#  FREETYPE_INCLUDE_DIR - The freetype include directory
#  FREETYPE_LIBRARIES - The libraries needed to use freetype
#  FREETYPE_DEFINITIONS - Compiler switches required for using freetype

SET(FREETYPE_DEFINITIONS ${PC_FREETYPE_CFLAGS_OTHER})

FIND_PATH(FREETYPE_INCLUDE_DIR ft2build.h
   HINTS
   ${PC_FREETYPE_INCLUDEDIR}
   ${PC_FREETYPE_INCLUDE_DIRS}
   PATH_SUFFIXES freetype2
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(FREETYPE_LIBRARIES NAMES freetype
   HINTS
   ${PC_FREETYPE_LIBDIR}
   ${PC_FREETYPE_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(FREETYPE_LIBRARIES ${FREETYPE_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREETYPE DEFAULT_MSG FREETYPE_LIBRARIES FREETYPE_INCLUDE_DIR)

MARK_AS_ADVANCED(FREETYPE_INCLUDE_DIR FREETYPE_LIBRARIES)
