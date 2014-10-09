project (netc)

if (NOT WIN32 AND NOT APPLE)
    include (FindX11)   
endif ()

set (AUTO_FILES_RESULT)
if (MSVC)
    AutoFiles("." "res" "\\.(rc)$")
endif ()
AutoFiles("." "src" "\\.(cpp|c|h)$")

if (MSVC)
    add_executable (netc WIN32 ${AUTO_FILES_RESULT})
else ()
    add_executable (netc ${AUTO_FILES_RESULT})
endif ()

target_link_libraries (netc buildin)

target_link_libraries (netc
    ${LUA_LIBRARIES}
    ${LIBEVENT_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${GLFW3_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${FREETYPE_LIBRARIES}
    ${GLEW_LIBRARIES}
)

include_directories (
    ${LUA_INCLUDE_DIR}
    ${LIBEVENT_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

if (WIN32)
    target_link_libraries (netc ws2_32 winmm gdi32 kernel32 user32 imm32 opengl32)
endif ()

create_target_launcher(netc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
create_default_target_launcher(netc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})