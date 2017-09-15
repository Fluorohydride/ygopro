solution "ygo"
    location "build"
    language "C++"
    objdir "obj"

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
        flags { "StaticRuntime", "LinkTimeOptimization" }
        disablewarnings { "4244", "4267", "4838", "4577", "4819", "4018", "4996", "4477" }

    configuration { "Release", "not vs*" }
        symbols "On"
        defines "NDEBUG"
        buildoptions "-march=native"

    configuration { "Debug", "vs*" }
        defines { "_ITERATOR_DEBUG_LEVEL=0" }

    configuration "vs*"
        flags "EnableSSE2"
        defines { "_CRT_SECURE_NO_WARNINGS" }

    configuration "not vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

    configuration {"not vs*", "windows"}
        buildoptions { "-static-libgcc" }

    startproject "ygopro"

    include "ocgcore"
    include "gframe"
    if os.is("windows") then
    include "event"
    include "lua"
    include "sqlite3"
    end
