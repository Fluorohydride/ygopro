project (ygopro)

if (NOT WIN32 AND NOT APPLE)
    include (FindX11)   
endif ()

set (AUTO_FILES_RESULT)
if (MSVC)
    AutoFiles("." "res" "\\.(rc)$")
    AutoFiles("." "src" "\\.(cpp|c|h)$" "CGUIButton.cpp")
else ()
    AutoFiles("." "src" "\\.(cpp|c|h)$")
endif ()

if (MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
    add_executable (ygopro WIN32 ${AUTO_FILES_RESULT})
else ()
    add_executable (ygopro ${AUTO_FILES_RESULT})
endif ()

target_link_libraries (ygopro ocgcore)

if (MSVC)
    target_link_libraries (ygopro opengl32 irrlicht freetype sqlite3 lua event)
    include_directories ( "../irrlicht/include" "../freetype/include" "../event/include" "../sqlite3" )
else ()
    target_link_libraries (ygopro ${FREETYPE_LIBRARY} ${LUA_LIBRARIES}
                                  ${LIBEVENT_LIBRARIES} ${IRRLICHT_LIBRARIES}
                                  ${SQLITE3_LIBRARIES} ${OPENGL_gl_LIBRARY})
    include_directories (${FREETYPE_INCLUDE_DIRS} ${LUA_INCLUDE_DIR}
                         ${LIBEVENT_INCLUDE_DIR} ${IRRLICHT_INCLUDE_DIR}
                         ${SQLITE3_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
    target_link_libraries (ygopro ${CMAKE_THREAD_LIBS_INIT} ${DL_LIBRARIES})
endif ()
if (WIN32)
    target_link_libraries (ygopro ws2_32 winmm gdi32 kernel32 user32 imm32)
endif ()

create_target_launcher(ygopro WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
create_default_target_launcher(ygopro WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
