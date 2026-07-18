-- Supported systems: Windows, Linux, MacOS

-- Windows (Visual Studio) build supports x86, x86_64, and ARM64.
-- Linux build supports x86_64 and ARM64.
-- MacOS build supports x86_64 and ARM64, and it supports cross-compilation.

-- Global settings

-- Default: Build Lua, Irrlicht, miniaudio from source on all systems.
--          Don't build event, freetype, sqlite, jpeg, png, zlib, opus, vorbis on Linux or MacOS, use package manager,
--          but build them on Windows, due to the lack of package manager on Windows.

BUILD_LUA = true
LUA_LIB_NAME = "lua" -- change this if you don't build Lua

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

-- Read settings from command line or environment variables
-- Default values should be defined at the top of the script. If any values are set in the premake options, GetParam will not
-- read them from environment variables.

newoption { trigger = "build-lua", category = "YGOPro - lua", description = "" }
newoption { trigger = "no-build-lua", category = "YGOPro - lua", description = "" }
newoption { trigger = "lua-include-dir", category = "YGOPro - lua", description = "", value = "PATH" }
newoption { trigger = "lua-lib-dir", category = "YGOPro - lua", description = "", value = "PATH" }
newoption { trigger = "lua-lib-name", category = "YGOPro - lua", description = "", value = "NAME" }

newoption { trigger = "build-event", category = "YGOPro - event", description = "" }
newoption { trigger = "no-build-event", category = "YGOPro - event", description = "" }
newoption { trigger = "event-include-dir", category = "YGOPro - event", description = "", value = "PATH" }
newoption { trigger = "event-lib-dir", category = "YGOPro - event", description = "", value = "PATH" }

newoption { trigger = "build-freetype", category = "YGOPro - freetype", description = "" }
newoption { trigger = "no-build-freetype", category = "YGOPro - freetype", description = "" }
newoption { trigger = "freetype-include-dir", category = "YGOPro - freetype", description = "", value = "PATH" }
newoption { trigger = "freetype-lib-dir", category = "YGOPro - freetype", description = "", value = "PATH" }

newoption { trigger = "build-sqlite", category = "YGOPro - sqlite", description = "" }
newoption { trigger = "no-build-sqlite", category = "YGOPro - sqlite", description = "" }
newoption { trigger = "sqlite-include-dir", category = "YGOPro - sqlite", description = "", value = "PATH" }
newoption { trigger = "sqlite-lib-dir", category = "YGOPro - sqlite", description = "", value = "PATH" }

newoption { trigger = "build-irrlicht", category = "YGOPro - irrlicht", description = "" }
newoption { trigger = "no-build-irrlicht", category = "YGOPro - irrlicht", description = "" }
newoption { trigger = "irrlicht-include-dir", category = "YGOPro - irrlicht", description = "", value = "PATH" }
newoption { trigger = "irrlicht-lib-dir", category = "YGOPro - irrlicht", description = "", value = "PATH" }
newoption { trigger = "no-dxsdk", category = "YGOPro - irrlicht", description = "" }

newoption { trigger = "build-jpeg", category = "YGOPro - jpeg", description = "" }
newoption { trigger = "no-build-jpeg", category = "YGOPro - jpeg", description = "" }
newoption { trigger = "jpeg-include-dir", category = "YGOPro - jpeg", description = "", value = "PATH" }
newoption { trigger = "jpeg-lib-dir", category = "YGOPro - jpeg", description = "", value = "PATH" }
newoption { trigger = "jpeg-lib-name", category = "YGOPro - jpeg", description = "", value = "NAME" }

newoption { trigger = "build-png", category = "YGOPro - png", description = "" }
newoption { trigger = "no-build-png", category = "YGOPro - png", description = "" }
newoption { trigger = "png-include-dir", category = "YGOPro - png", description = "", value = "PATH" }
newoption { trigger = "png-lib-dir", category = "YGOPro - png", description = "", value = "PATH" }

newoption { trigger = "build-zlib", category = "YGOPro - zlib", description = "" }
newoption { trigger = "no-build-zlib", category = "YGOPro - zlib", description = "" }
newoption { trigger = "zlib-include-dir", category = "YGOPro - zlib", description = "", value = "PATH" }
newoption { trigger = "zlib-lib-dir", category = "YGOPro - zlib", description = "", value = "PATH" }
newoption { trigger = "zlib-lib-name", category = "YGOPro - zlib", description = "", value = "NAME" }

newoption { trigger = "no-audio", category = "YGOPro", description = "" }
newoption { trigger = "audio-lib", category = "YGOPro", description = "", value = "" }

newoption { trigger = "miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "opus-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "opus-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "opusfile-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "opusfile-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "vorbis-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "vorbis-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "ogg-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "ogg-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }

newoption { trigger = "build-lzma", category = "YGOPro - lzma", description = "" }
newoption { trigger = "no-build-lzma", category = "YGOPro - lzma", description = "" }
newoption { trigger = "lzma-include-dir", category = "YGOPro - lzma", description = "", value = "PATH" }
newoption { trigger = "lzma-lib-dir", category = "YGOPro - lzma", description = "", value = "PATH" }

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

if GetParam("build-lua") then
    BUILD_LUA = true
elseif GetParam("no-build-lua") then
    BUILD_LUA = false
end
if not BUILD_LUA then
    -- at most times you need to change those if you change BUILD_LUA to false
    -- make sure your lua lib is built with C++ and version >= 5.3
    LUA_LIB_NAME = GetParam("lua-lib-name") or LUA_LIB_NAME
    LUA_INCLUDE_DIR = GetParam("lua-include-dir") or os.findheader("lua.h")
    LUA_LIB_DIR = GetParam("lua-lib-dir") or os.findlib(LUA_LIB_NAME)
end

if GetParam("build-event") then
    BUILD_EVENT = true
elseif GetParam("no-build-event") then
    BUILD_EVENT = false
end
if not BUILD_EVENT then
    EVENT_INCLUDE_DIR = GetParam("event-include-dir") or os.findheader("event2/event.h")
    EVENT_LIB_DIR = GetParam("event-lib-dir") or os.findlib("event")
end

if GetParam("build-freetype") then
    BUILD_FREETYPE = true
elseif GetParam("no-build-freetype") then
    BUILD_FREETYPE = false
end
if not BUILD_FREETYPE then
    FREETYPE_INCLUDE_DIR = GetParam("freetype-include-dir") or FindHeaderWithSubDir("freetype2/ft2build.h", "freetype2")
    FREETYPE_LIB_DIR = GetParam("freetype-lib-dir") or os.findlib("freetype")
end

if GetParam("build-sqlite") then
    BUILD_SQLITE = true
elseif GetParam("no-build-sqlite") then
    BUILD_SQLITE = false
end
if not BUILD_SQLITE then
    SQLITE_INCLUDE_DIR = GetParam("sqlite-include-dir") or os.findheader("sqlite3.h")
    SQLITE_LIB_DIR = GetParam("sqlite-lib-dir") or os.findlib("sqlite3")
end

if GetParam("build-lzma") then
    BUILD_LZMA = true
elseif GetParam("no-build-lzma") then
    BUILD_LZMA = false
end
if not BUILD_LZMA then
    LZMA_INCLUDE_DIR = GetParam("lzma-include-dir") or os.findheader("lzma.h")
    LZMA_LIB_DIR = GetParam("lzma-lib-dir") or os.findlib("lzma")
end

if GetParam("build-irrlicht") then
    BUILD_IRRLICHT = true
elseif GetParam("no-build-irrlicht") then
    BUILD_IRRLICHT = false
end
if not BUILD_IRRLICHT then
    IRRLICHT_INCLUDE_DIR = GetParam("irrlicht-include-dir") or os.findheader("irrlicht.h")
    IRRLICHT_LIB_DIR = GetParam("irrlicht-lib-dir") or os.findlib("irrlicht")
end

if GetParam("build-jpeg") then
    BUILD_JPEG = true
elseif GetParam("no-build-jpeg") then
    BUILD_JPEG = false
end
if not BUILD_JPEG then
    JPEG_LIB_NAME = GetParam("jpeg-lib-name") or JPEG_LIB_NAME
    JPEG_INCLUDE_DIR = GetParam("jpeg-include-dir") or os.findheader("jpeglib.h")
    JPEG_LIB_DIR = GetParam("jpeg-lib-dir") or os.findlib(JPEG_LIB_NAME)
end

if GetParam("build-png") then
    BUILD_PNG = true
elseif GetParam("no-build-png") then
    BUILD_PNG = false
end
if not BUILD_PNG then
    PNG_INCLUDE_DIR = GetParam("png-include-dir") or os.findheader("png.h")
    PNG_LIB_DIR = GetParam("png-lib-dir") or os.findlib("png")
end

if GetParam("build-zlib") then
    BUILD_ZLIB = true
elseif GetParam("no-build-zlib") then
    BUILD_ZLIB = false
end
if not BUILD_ZLIB then
    ZLIB_LIB_NAME = GetParam("zlib-lib-name") or ZLIB_LIB_NAME
    ZLIB_INCLUDE_DIR = GetParam("zlib-include-dir") or os.findheader("zlib.h")
    ZLIB_LIB_DIR = GetParam("zlib-lib-dir") or os.findlib(ZLIB_LIB_NAME)
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
                OPUS_INCLUDE_DIR = GetParam("opus-include-dir") or FindHeaderWithSubDir("opus/opus.h", "opus")
                OPUS_LIB_DIR = GetParam("opus-lib-dir") or os.findlib("opus")
                OPUSFILE_INCLUDE_DIR = GetParam("opusfile-include-dir") or FindHeaderWithSubDir("opus/opusfile.h", "opus")
                OPUSFILE_LIB_DIR = GetParam("opusfile-lib-dir") or os.findlib("opusfile")
                VORBIS_INCLUDE_DIR = GetParam("vorbis-include-dir") or os.findheader("vorbis/vorbisfile.h")
                VORBIS_LIB_DIR = GetParam("vorbis-lib-dir") or os.findlib("vorbis")
                OGG_INCLUDE_DIR = GetParam("ogg-include-dir") or os.findheader("ogg/ogg.h")
                OGG_LIB_DIR = GetParam("ogg-lib-dir") or os.findlib("ogg")
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
        -- Target Windows 7 or later. (Building requires Windows 10 SDK 1803 or newer.)
        defines { "WINVER=0x0601", "_WIN32_WINNT=0x0601" }

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
