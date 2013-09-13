project (ocgcore)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

if (WIN32)
    
else ()
    include_directories ( ${LUA_INCLUDE_DIR} )
endif ()

add_library (ocgcore STATIC ${AUTO_FILES_RESULT})