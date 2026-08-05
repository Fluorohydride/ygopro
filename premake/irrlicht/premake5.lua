project "irrlicht"
    kind "StaticLib"

    includedirs { "include", JPEG_INCLUDE_DIR, PNG_INCLUDE_DIR, ZLIB_INCLUDE_DIR }

    exceptionhandling "Off"
    rtti "Off"
    dofile("defines.lua")

    files {
        "include/*.h",
        "source/Irrlicht/*.cpp",
    }

    filter { "system:windows" }
        if USE_DXSDK then
            includedirs { "$(DXSDK_DIR)Include" }
            defines { "IRR_COMPILE_WITH_DX9_DEV_PACK" }
        else
            defines { "NO_IRR_COMPILE_WITH_DIRECT3D_9_" }
        end

    filter { "system:macosx" }
        cppdialect "gnu++14"
        defines { "GL_SILENCE_DEPRECATION" }
        files {
            "source/Irrlicht/*.mm",
        }
        buildoptions { "-Wno-deprecated-declarations" }

    filter { "system:macosx", "files:source/Irrlicht/Irrlicht.cpp or source/Irrlicht/COSOperator.cpp" }
        compileas "Objective-C++" 
