project (irrlicht)

set (AUTO_FILES_RESULT)
AutoFiles("." "src" "\\.(c|cpp|h)$")

include_directories ( include src/zlib src/jpeglib src/libpng )

add_definitions ("-D_IRR_STATIC_LIB_")

if (WIN32)
  add_definitions ("-DIRRLICHT_FAST_MATH")
endif ()

add_library (irrlicht STATIC ${AUTO_FILES_RESULT})
