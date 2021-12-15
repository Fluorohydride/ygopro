solution "ygo"
    location "build"
    language "C++"
    objdir "obj"

    if os.ishost("windows") then
        BUILD_LUA=true
    end

    configurations { "Release", "Debug" }

    configuration "windows"
        defines { "WIN32", "_WIN32" }

    configuration "linux"
        defines { "LUA_USE_LINUX" }

    configuration "Release"
        targetdir "bin/release"

    configuration "Debug"
        symbols "On"
        defines "_DEBUG"
        targetdir "bin/debug"

    configuration { "Release", "vs*" }
        optimize "Speed"
        flags { "LinkTimeOptimization" }
        staticruntime "On"
        disablewarnings { "4244", "4267", "4838", "4577", "4819", "4018", "4996", "4477", "4091", "4828", "4800" }

    configuration { "Release", "not vs*" }
        symbols "On"
        defines "NDEBUG"
        buildoptions "-march=native"

    configuration { "Debug", "vs*" }
        defines { "_ITERATOR_DEBUG_LEVEL=0" }

    configuration "vs*"
        vectorextensions "SSE2"
        buildoptions { "/utf-8" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    configuration "not vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

    configuration {"not vs*", "windows"}
        buildoptions { "-static-libgcc" }

    startproject "ygopro"

    include "ocgcore"
    include "gframe"
    if os.ishost("windows") then
    include "lua"
    include "event"
    include "sqlite3"
    end
