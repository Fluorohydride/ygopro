# - Try to find the Lib library
# Once done this will define
#
#  CEGUI_FOUND - System has cegui
#  CEGUI_INCLUDE_DIR - The cegui include directory
#  CEGUI_LIBRARIES - The libraries needed to use cegui
#  CEGUI_DEFINITIONS - Compiler switches required for using cegui

SET(CEGUI_DEFINITIONS ${PC_CEGUI_CFLAGS_OTHER})

FIND_PATH(CEGUI_INCLUDE_DIR CEGUI/CEGUI.h
   HINTS
   ${PC_CEGUI_INCLUDEDIR}
   ${PC_CEGUI_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(CEGUI_LIBRARIES1 NAMES CEGUIBase-0
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(CEGUI_LIBRARIES2 NAMES CEGUISTBImageCodec
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(CEGUI_LIBRARIES3 NAMES CEGUIOpenGLRenderer-0
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(CEGUI_LIBRARIES4 NAMES CEGUICoreWindowRendererSet
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(CEGUI_LIBRARIES5 NAMES CEGUIExpatParser
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(CEGUI_LIBRARIES ${CEGUI_LIBRARIES1} ${CEGUI_LIBRARIES2} ${CEGUI_LIBRARIES3} ${CEGUI_LIBRARIES4} ${CEGUI_LIBRARIES5})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CEGUI DEFAULT_MSG CEGUI_LIBRARIES CEGUI_INCLUDE_DIR)

MARK_AS_ADVANCED(CEGUI_INCLUDE_DIR CEGUI_LIBRARIES)
