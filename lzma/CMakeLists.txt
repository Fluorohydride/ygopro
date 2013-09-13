project (lzma C)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

add_library (lzma STATIC ${AUTO_FILES_RESULT})