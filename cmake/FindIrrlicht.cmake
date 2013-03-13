# - Try to find the Lib library
# Once done this will define
#
#  IRRLICHT_FOUND - System has IRRLICHT
#  IRRLICHT_INCLUDE_DIR - The IRRLICHT include directory
#  IRRLICHT_LIBRARIES - The libraries needed to use IRRLICHT
#  IRRLICHT_DEFINITIONS - Compiler switches required for using IRRLICHT


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#FIND_PACKAGE(PkgConfig)
#PKG_SEARCH_MODULE(PC_IRRLICHT Irrlicht)

SET(IRRLICHT_DEFINITIONS ${PC_IRRLICHT_CFLAGS_OTHER})

FIND_PATH(IRRLICHT_INCLUDE_DIR irrlicht.h
   HINTS
   ${PC_IRRLICHT_INCLUDEDIR}
   ${PC_IRRLICHT_INCLUDE_DIRS}
   PATH_SUFFIXES include include/irrlicht
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(IRRLICHT_LIBRARIES NAMES Irrlicht irrlicht
   HINTS
   ${PC_IRRLICHT_LIBDIR}
   ${PC_IRRLICHT_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Irrlicht DEFAULT_MSG IRRLICHT_LIBRARIES IRRLICHT_INCLUDE_DIR)

MARK_AS_ADVANCED(IRRLICHT_INCLUDE_DIR IRRLICHT_LIBRARIES)
