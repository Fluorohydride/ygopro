project "Irrlicht"
    kind "StaticLib"

    includedirs { "include", "src/zlib", "src/jpeglib", "src/libpng" }
    defines { "_IRR_STATIC_LIB_" }
    flags { "NoExceptions", "NoRTTI" }
    files { "**.cpp", "**.c", "**.cxx", "**.hpp", "**.h" }
    configuration { "vs*" }
        defines { "IRRLICHT_FAST_MATH" }
    configuration { "windows" }
        links { "imm32" }
