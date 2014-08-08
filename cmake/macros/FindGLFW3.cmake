# - Try to find the Lib library
# Once done this will define
#
#  GLFW3_FOUND - System has glfw3
#  GLFW3_INCLUDE_DIR - The glfw3 include directory
#  GLFW3_LIBRARIES - The libraries needed to use glfw3
#  GLFW3_DEFINITIONS - Compiler switches required for using glfw3

SET(GLFW3_DEFINITIONS ${PC_GLFW3_CFLAGS_OTHER})

FIND_PATH(GLFW3_INCLUDE_DIR GLFW/glfw3.h
   HINTS
   ${PC_GLFW3_INCLUDEDIR}
   ${PC_GLFW3_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(GLFW3_LIBRARIES NAMES GLFW3
   HINTS
   ${PC_GLFW3_LIBDIR}
   ${PC_GLFW3_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(GLFW3_LIBRARIES ${GLFW3_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG GLFW3_LIBRARIES GLFW3_INCLUDE_DIR)

MARK_AS_ADVANCED(GLFW3_INCLUDE_DIR GLFW3_LIBRARIES)
