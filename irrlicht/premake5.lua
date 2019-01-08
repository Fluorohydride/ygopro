project "Irrlicht"
    kind "StaticLib"

    includedirs { "include", "src/zlib", "src/jpeglib", "src/libpng", "$(DXSDK_DIR)Include" } 
    libdirs { "$(DXSDK_DIR)Lib/x86" }
    defines { "_IRR_STATIC_LIB_" }
	exceptionhandling "Off"
	rtti "Off"
    files { "**.cpp", "**.c", "**.cxx", "**.hpp", "**.h" }
    configuration { "vs*" }
        defines { "IRRLICHT_FAST_MATH", "UNICODE", "_UNICODE" }
    configuration { "windows" }
        links { "imm32" }
