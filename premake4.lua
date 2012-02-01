solution "ygo"
    location "build"
    language "C++"
    objdir "obj"

    configurations { "Debug", "Release" }

    configuration "windows"
        defines { "WIN32", "_WIN32" }

    configuration "bsd"
        includedirs { "/usr/local/include" }
        libdirs { "/usr/local/lib" }

    configuration "macosx"
        includedirs { "/opt/local/include" }
        libdirs { "/opt/local/lib" }

    configuration "vs*"
        flags "EnableSSE2"
        buildoptions { "-wd4996" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    configuration "not vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }
    configuration {"not vs*", "windows"}
        buildoptions { "-static-libgcc" }

    configuration "Debug"
        flags "Symbols"
        defines "_DEBUG"
        targetdir "bin/debug"

    configuration { "Release", "not vs*" }
        flags "Symbols"
	defines "NDEBUG"
        buildoptions "-march=native"

    configuration "Release"
        flags { "OptimizeSpeed" }
        targetdir "bin/release"

    include "ocgcore"
    include "gframe"
