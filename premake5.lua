-- Supported systems: Windows, Linux, MacOS

-- Windows (Visual Studio) build supports x86, x86_64, and ARM64.
-- Linux build supports x86_64 and ARM64.
-- MacOS build supports x86_64 and ARM64, and it supports cross-compilation.

-- Global settings

-- Default: Build Lua, Irrlicht, miniaudio from source on all systems.
--          Don't build event, freetype, sqlite, jpeg, png, zlib, opus, vorbis on Linux or MacOS, use package manager,
--          but build them on Windows, due to the lack of package manager on Windows.

BUILD_LUA = true
LUA_LIB_NAME = "lua" -- at most times you need to change those if you don't build Lua from source:
                     -- most Lua distributions provide a C lib named "lua", but ocgcore requires lua built as C++.

BUILD_EVENT = os.istarget("windows")

BUILD_FREETYPE = os.istarget("windows")

BUILD_SQLITE = os.istarget("windows")

BUILD_IRRLICHT = true -- modified Irrlicht is required, can't use the official one
USE_DXSDK = true

BUILD_JPEG = os.istarget("windows") -- libjpeg-turbo is required, can't use the bundled IJG jpeglib from Irrlicht
JPEG_LIB_NAME = "jpeg" -- use the libjpeg API of libjpeg-turbo, the lib name should always be "jpeg", just in case

BUILD_PNG = os.istarget("windows")

BUILD_ZLIB = os.istarget("windows")
ZLIB_LIB_NAME = "z" -- the lib name should always be "z", just in case

USE_AUDIO = true
AUDIO_LIB = "miniaudio" -- only miniaudio is supported for now
-- BUILD_MINIAUDIO is always true
MINIAUDIO_SUPPORT_OPUS_VORBIS = true
MINIAUDIO_BUILD_OPUS_VORBIS = os.istarget("windows")

BUILD_LZMA = os.istarget("windows")

-- Available: none, sse2, avx2, neon, best
-- "best" means avx2 on x86 and neon on ARM
USE_SIMD = "best"

-- Variable indicating whether we are building for Apple Silicon, will be detected automatically if not specified.
local MAC_ARM = false
local MAC_INTEL = false

-- os.hostarch() actually returns the architecture of Premake5, and the official Windows build of Premake5 is 32-bit,
-- so we can only distinguish between AARCH64 and x86, and must use the ARM build of Premake5 on ARM platforms.
PREMAKE_ARCH = os.hostarch()

-- Default include dirs, those values are ONLY used in static builds, WILL BE IGNORED if set corresponding BUILD_* to false
LUA_INCLUDE_DIR = path.getabsolute("./lua/src")
EVENT_INCLUDE_DIR = path.getabsolute("./event/include")
IRRLICHT_INCLUDE_DIR = path.getabsolute("./irrlicht/include")
JPEG_INCLUDE_DIR = path.getabsolute("./jpeg/src")
PNG_INCLUDE_DIR = path.getabsolute("./png")
ZLIB_INCLUDE_DIR = path.getabsolute("./zlib")
FREETYPE_CUSTOM_INCLUDE_DIR = path.getabsolute("./freetype/custom")
FREETYPE_INCLUDE_DIR = path.getabsolute("./freetype/include")
SQLITE_INCLUDE_DIR = path.getabsolute("./sqlite3")
MINIAUDIO_INCLUDE_DIR = path.getabsolute("./miniaudio")
MINIAUDIO_OPUS_INCLUDE_DIR = path.getabsolute("./miniaudio/extras/decoders/libopus")
MINIAUDIO_VORBIS_INCLUDE_DIR = path.getabsolute("./miniaudio/extras/decoders/libvorbis")
LZMA_INCLUDE_DIR = path.getabsolute("./lzma/src/liblzma/api")

-- Fields: name, header, header_subdir (for FindHeaderWithSubDir), findlib (override lib name), libname_var (global holding the lib name)
local buildDeps = {
    { name = "lua",      header = "lua.h",                libname_var = "LUA_LIB_NAME"  },
    { name = "event",    header = "event2/event.h"                                      },
    { name = "freetype", header = "freetype2/ft2build.h", header_subdir = "freetype2"   },
    { name = "sqlite",   header = "sqlite3.h",            findlib = "sqlite3"           },
    { name = "lzma",     header = "lzma.h"                                              },
    { name = "irrlicht", header = "irrlicht.h"                                          },
    { name = "jpeg",     header = "jpeglib.h",            libname_var = "JPEG_LIB_NAME" },
    { name = "png",      header = "png.h"                                               },
    { name = "zlib",     header = "zlib.h",               libname_var = "ZLIB_LIB_NAME" },
}
-- Fields: name, header, header_subdir (for FindHeaderWithSubDir)
local miniaudioDeps = {
    { name = "opus",     header = "opus/opus.h",          header_subdir = "opus"        },
    { name = "opusfile", header = "opus/opusfile.h",      header_subdir = "opus"        },
    { name = "vorbis",   header = "vorbis/vorbisfile.h"                                 },
    { name = "ogg",      header = "ogg/ogg.h"                                           },
}

-- Read settings from command line or environment variables
-- Default values should be defined at the top of the script.
-- If any values are set in the premake options, GetParam will not read them from environment variables.

for _, dep in ipairs(buildDeps) do
    local name = dep.name
    local cat  = "YGOPro - " .. name
    newoption { trigger = "build-"    .. name,       category = cat, description = "" }
    newoption { trigger = "no-build-" .. name,       category = cat, description = "" }
    newoption { trigger = name .. "-include-dir",    category = cat, description = "", value = "PATH" }
    newoption { trigger = name .. "-lib-dir",        category = cat, description = "", value = "PATH" }
    if dep.libname_var then
        newoption { trigger = name .. "-lib-name",   category = cat, description = "", value = "NAME" }
    end
end
for _, dep in ipairs(miniaudioDeps) do
    newoption { trigger = dep.name .. "-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
    newoption { trigger = dep.name .. "-lib-dir",     category = "YGOPro - miniaudio", description = "", value = "PATH" }
end

newoption { trigger = "no-dxsdk", category = "YGOPro - irrlicht", description = "" }

newoption { trigger = "no-audio", category = "YGOPro", description = "" }
newoption { trigger = "audio-lib", category = "YGOPro", description = "", value = "" }

newoption { trigger = "miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }

newoption { trigger = "vs2026-win7-support", category = "YGOPro", description = "Enable Windows 7 support (toolset v143) for Visual Studio 2026" }

newoption { trigger = "mac-arm", category = "YGOPro", description = "Cross Compile for Apple Silicon Mac" }
newoption { trigger = "mac-intel", category = "YGOPro", description = "Cross Compile for Intel Mac" }

newoption { trigger = "use-openmp", category = "YGOPro", description = "Enable OpenMP support (edge case)" }

newoption { trigger = "use-simd", category = "YGOPro", description = "", value = "none, sse2, avx2, neon, best" }

function GetParam(param)
    return _OPTIONS[param] or os.getenv(string.upper(string.gsub(param,"-","_")))
end

function FindHeaderWithSubDir(header, subdir)
    local result = os.findheader(header)
    if result and subdir then
        result = path.join(result, subdir)
    end
    return result
end

function CheckDirectory(varname)
    local dir = _G[varname]
    if not dir or dir == "" then
        print("::warning:: " .. varname .. " is not set")
    elseif not os.isdir(dir) then
        print("::warning:: " .. varname .. " is not a valid directory: " .. dir)
    end
end

function GetDependencyDirectory(dep)
    local upper = string.upper(dep.name)
    local findlib_name = dep.findlib or (dep.libname_var and _G[dep.libname_var]) or dep.name
    local include_dir_var = upper .. "_INCLUDE_DIR"
    local lib_dir_var = upper .. "_LIB_DIR"
    _G[include_dir_var] = GetParam(dep.name .. "-include-dir") or FindHeaderWithSubDir(dep.header, dep.header_subdir)
    _G[lib_dir_var] = GetParam(dep.name .. "-lib-dir") or os.findlib(findlib_name)
    CheckDirectory(include_dir_var)
    CheckDirectory(lib_dir_var)
end

-- Process build flags and external directory settings for all library dependencies.
for _, dep in ipairs(buildDeps) do
    local name  = dep.name
    local upper = string.upper(name)
    local flag  = "BUILD_" .. upper
    if GetParam("build-" .. name) then
        _G[flag] = true
    elseif GetParam("no-build-" .. name) then
        _G[flag] = false
    end
    if not _G[flag] then
        if dep.libname_var then
            _G[dep.libname_var] = GetParam(name .. "-lib-name") or _G[dep.libname_var]
        end
        GetDependencyDirectory(dep)
    end
end

if GetParam("no-dxsdk") then
    USE_DXSDK = false
end
if USE_DXSDK and os.istarget("windows") then
    if not os.getenv("DXSDK_DIR") then
        print("Warning: DXSDK_DIR environment variable not set, it seems you don't have the DirectX SDK installed. DirectX mode will be disabled.")
        USE_DXSDK = false
    end
end

if GetParam("no-audio") then
    USE_AUDIO = false
elseif GetParam("no-use-miniaudio") then
    print("Warning: --no-use-miniaudio is deprecated, use --no-audio")
    USE_AUDIO = false
elseif GetParam("use-miniaudio") then
    print("Warning: --use-miniaudio is deprecated, use --audio-lib=miniaudio")
    USE_AUDIO = true
    AUDIO_LIB = "miniaudio"
end

if USE_AUDIO then
    AUDIO_LIB = GetParam("audio-lib") or AUDIO_LIB
    if AUDIO_LIB == "miniaudio" then
        if GetParam("miniaudio-support-opus-vorbis") then
            MINIAUDIO_SUPPORT_OPUS_VORBIS = true
        elseif GetParam("no-miniaudio-support-opus-vorbis") then
            MINIAUDIO_SUPPORT_OPUS_VORBIS = false
        end
        if MINIAUDIO_SUPPORT_OPUS_VORBIS then
            if GetParam("no-build-opus-vorbis") then
                MINIAUDIO_BUILD_OPUS_VORBIS = false
            elseif GetParam("build-opus-vorbis") then
                MINIAUDIO_BUILD_OPUS_VORBIS = true
            end
            if not MINIAUDIO_BUILD_OPUS_VORBIS then
                for _, dep in ipairs(miniaudioDeps) do
                    GetDependencyDirectory(dep)
                end
            end
        end
    else
        error("Unknown audio library: " .. AUDIO_LIB)
    end
end

USE_SIMD = GetParam("use-simd") or USE_SIMD

if not MAC_ARM and not MAC_INTEL and table.indexof({ "x86", "x86_64", "ARM64" }, PREMAKE_ARCH) == nil then
    print("Warning: Detected architecture " .. PREMAKE_ARCH .. " seems not supported, trying to build anyway, SIMD will be disabled.")
    USE_SIMD = "none"
end

if USE_SIMD == "avx2" or USE_SIMD == "neon" then
    USE_SIMD = "best"
end

if os.istarget("windows") and GetParam("vs2026-win7-support") then
    WIN7_SUPPORT = true
end

if os.istarget("macosx") then
    if GetParam("mac-arm") then
        MAC_ARM = true
    end
    if GetParam("mac-intel") then
        MAC_INTEL = true
    end
end

if GetParam("use-openmp") then
    USE_OPENMP = true
    if os.istarget("macosx") then
        print("Warning: OpenMP is not supported on Clang provided by Xcode.")
    end
end

workspace "YGOPro"
    location "build"
    language "C++"
    objdir "obj"

    configurations { "Release", "Debug" }

    filter "system:windows"
        systemversion "latest"
        startproject "YGOPro"
        defines { "WINVER=0x0601" } -- WIN7

    if WIN7_SUPPORT then
        filter { "system:windows", "action:vs2026" }
            toolset "v143"
    end

    filter { "system:windows", "action:vs*" }
        platforms { "Win32", "x64", "ARM64" }
        defaultplatform "x64"

    filter { "system:windows", "action:vs*", "platforms:Win32" }
        architecture "x86"
        if USE_SIMD == "none" then
            vectorextensions "IA32"
        end
        if USE_SIMD == "sse2" then
            vectorextensions "SSE2"
        end
        if USE_SIMD == "best" then
            vectorextensions "AVX2"
        end

    filter { "system:windows", "action:vs*", "platforms:x64" }
        architecture "x86_64"
        -- x86_64 must have SSE2, so we shouldn't check USE_SIMD for SSE2
        if USE_SIMD == "best" then
            vectorextensions "AVX2"
        end

    filter { "system:windows", "action:vs*", "platforms:ARM64" }
        architecture "AARCH64"

    filter "system:macosx"
        systemversion "11"
        if MAC_ARM and MAC_INTEL then
            print("Warning: Universal binary is no longer supported, please choose either --mac-arm or --mac-intel, and combine the binaries with lipo manually.")
            MAC_ARM = false
            MAC_INTEL = false
        end
        if not MAC_ARM and not MAC_INTEL then
            if PREMAKE_ARCH == "ARM64" then
                MAC_ARM = true
            else
                MAC_INTEL = true
            end
        end
        if MAC_ARM then
            architecture "AARCH64"
        end
        if MAC_INTEL then
            architecture "x86_64"
        end

    filter "system:linux"
        if PREMAKE_ARCH == "ARM64" then
            architecture "AARCH64"
        else
            architecture "x86_64"
        end

    filter "configurations:Release"
        optimize "Speed"
        targetdir "bin/release"

    filter "configurations:Debug"
        symbols "On"
        defines "_DEBUG"
        targetdir "bin/debug"

    filter { "system:windows", "platforms:Win32", "configurations:Release" }
        targetdir "bin/release/x86"

    filter { "system:windows", "platforms:Win32", "configurations:Debug" }
        targetdir "bin/debug/x86"

    filter { "system:windows", "platforms:x64", "configurations:Release" }
        targetdir "bin/release/x64"

    filter { "system:windows", "platforms:x64", "configurations:Debug" }
        targetdir "bin/debug/x64"

    filter { "system:windows", "platforms:ARM64", "configurations:Release" }
        targetdir "bin/release/arm64"

    filter { "system:windows", "platforms:ARM64", "configurations:Debug" }
        targetdir "bin/debug/arm64"

    filter { "configurations:Release", "action:vs*" }
        linktimeoptimization "On"
        staticruntime "On"
        disablewarnings { "4244", "4267", "4838", "4996", "6011", "6031", "6054", "6262" }

    filter { "configurations:Release", "not action:vs*" }
        defines "NDEBUG"

    filter { "configurations:Debug", "action:vs*" }
        disablewarnings { "6011", "6031", "6054", "6262" }

    filter "action:vs*"
        cdialect "C11"
        conformancemode "On" 
        buildoptions { "/utf-8" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "action:gmake"
        buildoptions { "-fno-strict-aliasing" }

    filter { "action:gmake", "architecture:x86_64" }
        if USE_SIMD == "best" then
            vectorextensions "AVX2"
            isaextensions { "FMA" }
        end

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
    if BUILD_JPEG then
        include "jpeg"
    end
    if BUILD_PNG then
        include "png"
    end
    if BUILD_ZLIB then
        include "zlib"
    end
    if BUILD_SQLITE then
        include "sqlite3"
    end
    if BUILD_LZMA then
        include "lzma/."
    end
    if USE_AUDIO then
        if AUDIO_LIB=="miniaudio" then
            include "miniaudio"
        end
    end
