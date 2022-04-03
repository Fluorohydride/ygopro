if os.ishost("windows") then
    USE_IRRKLANG = true
    IRRKLANG_PRO = false
    BUILD_LUA = true
    BUILD_EVENT = true
    BUILD_FREETYPE = true
    BUILD_IRRLICHT = true
    BUILD_SQLITE = true
else
    USE_IRRKLANG = true
    IRRKLANG_PRO = false
    BUILD_LUA = true
    BUILD_EVENT = false --not implemented on linux
    BUILD_FREETYPE = false
    BUILD_IRRLICHT = not os.ishost("macosx")
    BUILD_SQLITE = false
end

workspace "YGOPro"
    location "build"
    language "C++"
    objdir "obj"

    configurations { "Release", "Debug" }

    filter "system:windows"
        defines { "WIN32", "_WIN32", "WINVER=0x0501" }
        entrypoint "mainCRTStartup"
        systemversion "latest"
        startproject "ygopro"

    filter "system:bsd"
        includedirs { "/usr/local/include" }
        libdirs { "/usr/local/lib" }

    filter "system:macosx"
        includedirs { "/usr/local/include/freetype2" }
        libdirs { "/usr/local/lib" }
        buildoptions { "-stdlib=libc++" }
        links { "OpenGL.framework", "Cocoa.framework", "IOKit.framework" }

    filter "system:linux"
        buildoptions { "-U_FORTIFY_SOURCE" }

    filter "configurations:Release"
        optimize "Speed"
        targetdir "bin/release"

    filter "configurations:Debug"
        symbols "On"
        defines "_DEBUG"
        targetdir "bin/debug"

    filter { "configurations:Release", "action:vs*" }
        flags { "LinkTimeOptimization" }
        staticruntime "On"
        disablewarnings { "4244", "4267", "4838", "4577", "4819", "4018", "4996", "4477", "4091", "4828", "4800" }

    filter { "configurations:Release", "not action:vs*" }
        symbols "On"
        defines "NDEBUG"
        buildoptions "-march=native"

    filter { "configurations:Debug", "action:vs*" }
        defines { "_ITERATOR_DEBUG_LEVEL=0" }
        disablewarnings { "4819", "4828" }

    filter "action:vs*"
        vectorextensions "SSE2"
        buildoptions { "/utf-8" }
        defines { "_CRT_SECURE_NO_WARNINGS" }
    
    filter "not action:vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

    filter {"not action:vs*", "system:windows"}
        buildoptions { "-static-libgcc" }

    filter {}

    include "ocgcore"
    include "gframe"
    if BUILD_LUA then
        include "lua"
    end
    if BUILD_EVENT then
        include "event"
    end
    if BUILD_FREETYPE then
        include "freetype"
    end
    if BUILD_IRRLICHT then
        include "irrlicht"
    end
    if BUILD_SQLITE then
        include "sqlite3"
    end
    if USE_IRRKLANG and IRRKLANG_PRO then
        include "ikpmp3"
    end
