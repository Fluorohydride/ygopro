project (lua)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(cpp|c|h)$")

include_directories ( . )

add_library (lua STATIC ${AUTO_FILES_RESULT})
