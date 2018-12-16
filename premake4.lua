workspace "ygo"
    location "build"
    language "C++"
    objdir "obj"
	startproject "ygopro"

    configurations { "Debug", "DebugDLL" , "Release", "ReleaseDLL" }
	defines { "LUA_COMPAT_5_2" }

    configuration "windows"
        defines { "WIN32", "_WIN32", "NOMINMAX" }

    configuration "bsd"
        defines { "LUA_USE_POSIX" }
        includedirs { "/usr/local/include" }
        libdirs { "/usr/local/lib" }

    configuration "macosx"
        defines { "LUA_USE_MACOSX" }
        includedirs { "/opt/local/include" }
        libdirs { "/opt/local/lib" }

    configuration "linux"
        defines { "LUA_USE_LINUX" }

    configuration "vs*"
        vectorextensions "SSE2"
        buildoptions { "-wd4996" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    configuration "not vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }
    configuration {"not vs*", "windows"}
        buildoptions { "-static-libgcc" }

    configuration "Debug*"
        symbols "On"
        defines "_DEBUG"
        targetdir "bin/debug"
		
	configuration { "Release", "not vs*" }
        symbols "On"
        defines "NDEBUG"
        buildoptions "-march=native"

    configuration "Release*"
        optimize "Speed"
        targetdir "bin/release"
	configuration "*DLL"
		defines {"YGOPRO_BUILD_DLL"}

    include "ocgcore"
    include "gframe"
	include "fmt"
    if os.ishost("windows") then
    include "event"
    include "freetype"
    include "irrlicht"
    include "lua"
    include "sqlite3"
    end
