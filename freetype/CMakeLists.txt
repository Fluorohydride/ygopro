project (freetype C)
set (SRC_FILES1 "src/autofit/autofit.c" "src/bdf/bdf.c" "src/cff/cff.c" "src/base/ftbase.c"
            "src/cache/ftcache.c" "src/base/ftbitmap.c" "src/base/ftdebug.c" "src/base/ftfstype.c"
            "src/base/ftbbox.c" "src/base/ftgxval.c" "src/base/ftlcdfil.c" "src/base/ftmm.c"
            "src/base/ftotval.c" "src/base/ftpatent.c" "src/base/ftpfr.c" "src/base/ftsynth.c"
            "src/base/fttype1.c" "src/base/ftxf86.c" "src/base/ftgasp.c" "src/base/ftglyph.c"
            "src/base/ftinit.c" "src/base/ftstroke.c" "src/base/ftsystem.c" "src/smooth/smooth.c"
            "src/lzw/ftlzw.c" "src/gzip/ftgzip.c" "src/pcf/pcf.c" "src/pfr/pfr.c" "src/psaux/psaux.c"
            "src/pshinter/pshinter.c" "src/psnames/psmodule.c" "src/raster/raster.c" "src/sfnt/sfnt.c"
            "src/truetype/truetype.c" "src/type1/type1.c" "src/cid/type1cid.c" "src/type42/type42.c"
            "src/winfonts/winfnt.c")

source_group(src FILES ${SRC_FILES1})

add_definitions ("-DFT2_BUILD_LIBRARY")

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DFT_DEBUG_LEVEL_ERROR -DFT_DEBUG_LEVEL_TRACE")

include_directories ( include )

add_library (freetype STATIC ${SRC_FILES1})
