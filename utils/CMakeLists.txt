project (utils C)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h|hpp)$")

add_library (utils STATIC ${AUTO_FILES_RESULT})