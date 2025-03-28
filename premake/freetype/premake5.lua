project "freetype"
    kind "StaticLib"

    includedirs { "include" }
    defines { "FT2_BUILD_LIBRARY" }

    files { "src/autofit/autofit.c",
            "src/base/ftbase.c",
            "src/base/ftbitmap.c",
            "src/base/ftinit.c",
            "src/bdf/bdf.c",
            "src/cff/cff.c",
            "src/cid/type1cid.c",
            "src/gzip/ftgzip.c",
            "src/lzw/ftlzw.c",
            "src/pcf/pcf.c",
            "src/pfr/pfr.c",
            "src/psaux/psaux.c",
            "src/pshinter/pshinter.c",
            "src/psnames/psmodule.c",
            "src/raster/raster.c",
            "src/sfnt/sfnt.c",
            "src/sdf/sdf.c",
            "src/smooth/smooth.c",
            "src/truetype/truetype.c",
            "src/type1/type1.c",
            "src/type42/type42.c",
            "src/winfonts/winfnt.c" }


    if os.isfile("src/svg/svg.c") then
        files { "src/svg/svg.c" }
    end

    filter "system:windows"
        files { "builds/windows/ftsystem.c",
                "builds/windows/ftdebug.c" }

    filter "not system:windows"
        files { "src/base/ftsystem.c",
                "src/base/ftdebug.c" }
