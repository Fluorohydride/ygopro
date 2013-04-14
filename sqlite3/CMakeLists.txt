project (sqlite3 C)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

add_library (sqlite3 STATIC ${AUTO_FILES_RESULT})
