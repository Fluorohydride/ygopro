# - Try to find the Lib library
# Once done this will define
#
#  GLEW_FOUND - System has glew
#  GLEW_INCLUDE_DIR - The glew include directory
#  GLEW_LIBRARIES - The libraries needed to use glew
#  GLEW_DEFINITIONS - Compiler switches required for using glew

SET(GLEW_DEFINITIONS ${PC_GLEW_CFLAGS_OTHER})

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
   HINTS
   ${PC_GLEW_INCLUDEDIR}
   ${PC_GLEW_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(GLEW_LIBRARIES NAMES glew
   HINTS
   ${PC_GLEW_LIBDIR}
   ${PC_GLEW_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(GLEW_LIBRARIES ${GLEW_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG GLEW_LIBRARIES GLEW_INCLUDE_DIR)

MARK_AS_ADVANCED(GLEW_INCLUDE_DIR GLEW_LIBRARIES)
