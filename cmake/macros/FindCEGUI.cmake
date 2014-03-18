# - Try to find the Lib library
# Once done this will define
#
#  CEGUI_FOUND - System has Libevent
#  CEGUI_INCLUDE_DIR - The Libevent include directory
#  CEGUI_LIBRARIES - The libraries needed to use Libevent
#  CEGUI_DEFINITIONS - Compiler switches required for using Libevent

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

FIND_LIBRARY(CEGUI_LIBRARIES NAMES CEGUIBase-0 CEGUICommonDialogs-0 CEGUISTBImageCodec CEGUITGAImageCodec CEGUIOpenGLRenderer-0 CEGUICoreWindowRendererSet CEGUIExpatParser CEGUILibXMLParser CEGUINullRenderer-0
   HINTS
   ${PC_CEGUI_LIBDIR}
   ${PC_CEGUI_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

SET(CEGUI_LIBRARIES ${CEGUI_LIBRARIES})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CEGUI DEFAULT_MSG CEGUI_LIBRARIES CEGUI_INCLUDE_DIR)

MARK_AS_ADVANCED(CEGUI_INCLUDE_DIR CEGUI_LIBRARIES)
