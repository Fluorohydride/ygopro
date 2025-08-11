project "freetype"
    kind "StaticLib"

    includedirs { FirstPossibleDir("../premake/freetype/custom", "custom"), "include" }
    defines { "FT2_BUILD_LIBRARY" }

    files {
        "src/base/ftinit.c",
        "src/base/ftbase.c",
        "src/base/ftbitmap.c",
        "src/cff/cff.c",
        "src/psaux/psaux.c",
        "src/pshinter/pshinter.c",
        "src/psnames/psnames.c",
        "src/sfnt/sfnt.c",
        "src/smooth/smooth.c",
        "src/truetype/truetype.c",
    }

    filter "system:windows"
        files { "builds/windows/ftsystem.c",
                "builds/windows/ftdebug.c" }

    filter "not system:windows"
        files { "src/base/ftsystem.c",
                "src/base/ftdebug.c" }
