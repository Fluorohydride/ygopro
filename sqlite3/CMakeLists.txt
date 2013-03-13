project (sqlite3 C)
set (SRC_FILES1 sqlite3.c sqlite3.h)

if (MSVC)
    source_group(src FILES ${SRC_FILES1})
endif ()

add_library (sqlite3 STATIC ${SRC_FILES1})

include (MSVCMultipleProcessCompile)
