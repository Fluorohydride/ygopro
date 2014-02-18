project (nets)

AutoFiles("." "src" "\\.(cpp|c|h)$")

if (MSVC)
    add_executable (nets WIN32 ${AUTO_FILES_RESULT})
else ()
    add_executable (nets ${AUTO_FILES_RESULT})
endif ()

target_link_libraries (nets sqlite3 lzma ocgcore)

if (MSVC)
    target_link_libraries (nets sqlite3 lua event)
else ()
    target_link_libraries (nets ${LIBEVENT_LIBRARIES} ${LUA_LIBRARIES})
    include_directories (${LUA_INCLUDE_DIR} ${LIBEVENT_INCLUDE_DIR})
    target_link_libraries (nets ${CMAKE_THREAD_LIBS_INIT} ${DL_LIBRARIES})
endif ()

if (WIN32)
    target_link_libraries (nets ws2_32 winmm kernel32 user32 imm32)
endif ()

create_target_launcher(nets WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
create_default_target_launcher(nets WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})