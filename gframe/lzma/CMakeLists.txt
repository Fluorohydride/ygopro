project (clzma)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

add_library (clzma STATIC ${AUTO_FILES_RESULT})
