project (lua)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$" "lua.c|luac.c")

set_source_files_properties(${AUTO_FILES_RESULT} PROPERTIES LANGUAGE CXX)

add_library (lua STATIC ${AUTO_FILES_RESULT})
