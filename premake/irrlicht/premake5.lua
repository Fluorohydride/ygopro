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

    filter { "system:linux" }
        if IRR_BUILD_X11 then
            defines { "_IRR_COMPILE_WITH_X11_DEVICE_" }
        else
            defines { "NO_IRR_COMPILE_WITH_X11_DEVICE_" }
            removefiles {
                "source/Irrlicht/CIrrDeviceLinux.cpp",
                "source/Irrlicht/CGLXManager.cpp",
            }
        end
        if IRR_BUILD_WAYLAND then
            defines { "_IRR_COMPILE_WITH_WAYLAND_DEVICE_" }
            if IRR_WAYLAND_DIRECT_LINK then
                defines { "_IRR_COMPILE_WITH_WAYLAND_LINKED_DEVICE_" }
            end
        else
            removefiles {
                "source/Irrlicht/CIrrDeviceLinuxWayland.cpp",
                "source/Irrlicht/CEGLManagerWayland.cpp",
                "source/Irrlicht/CWaylandLibrary.cpp",
                "source/Irrlicht/CWaylandProtocols.cpp",
            }
        end

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
