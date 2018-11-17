# - Try to find the Lib library
# Once done this will define
#
#  LIBEVENT_FOUND - System has Libevent
#  LIBEVENT_INCLUDE_DIR - The Libevent include directory
#  LIBEVENT_LIBRARIES - The libraries needed to use Libevent
#  LIBEVENT_DEFINITIONS - Compiler switches required for using Libevent


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
#FIND_PACKAGE(PkgConfig)
#PKG_SEARCH_MODULE(PC_LIBEVENT event)

SET(LIBEVENT_DEFINITIONS ${PC_LIBEVENT_CFLAGS_OTHER})

FIND_PATH(LIBEVENT_INCLUDE_DIR event2/event.h
   HINTS
   ${PC_LIBEVENT_INCLUDEDIR}
   ${PC_LIBEVENT_INCLUDE_DIRS}
   PATH_SUFFIXES include
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(LIBEVENT_LIBRARIES NAMES event
   HINTS
   ${PC_LIBEVENT_LIBDIR}
   ${PC_LIBEVENT_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)

FIND_LIBRARY(LIBEVENT_LIBRARIES_PTHREADS NAMES event_pthreads
   HINTS
   ${PC_LIBEVENT_LIBDIR}
   ${PC_LIBEVENT_LIBRARY_DIRS}
   PATH_SUFFIXES lib64 lib
   PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /opt/local
)
SET(LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARIES} ${LIBEVENT_LIBRARIES_PTHREADS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibEvent DEFAULT_MSG LIBEVENT_LIBRARIES LIBEVENT_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBEVENT_INCLUDE_DIR LIBEVENT_LIBRARIES)
