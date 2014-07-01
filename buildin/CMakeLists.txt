project (buildin C)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

add_library (buildin STATIC ${AUTO_FILES_RESULT})