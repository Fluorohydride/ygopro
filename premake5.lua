-- default global settings

BUILD_LUA = true
LUA_LIB_NAME = "lua"

BUILD_EVENT = true
BUILD_FREETYPE = true
BUILD_SQLITE = true

BUILD_IRRLICHT = true

USE_AUDIO = true
AUDIO_LIB = "miniaudio"
-- BUILD_MINIAUDIO is always true
MINIAUDIO_SUPPORT_OPUS_VORBIS = true
MINIAUDIO_BUILD_OPUS_VORBIS = true
-- BUILD_IRRKLANG is impossible because irrKlang is not open source
IRRKLANG_PRO = false
IRRKLANG_PRO_BUILD_IKPMP3 = false

-- read settings from command line or environment variables

newoption { trigger = "build-lua", category = "YGOPro - lua", description = "" }
newoption { trigger = "no-build-lua", category = "YGOPro - lua", description = "" }
newoption { trigger = "lua-include-dir", category = "YGOPro - lua", description = "", value = "PATH" }
newoption { trigger = "lua-lib-dir", category = "YGOPro - lua", description = "", value = "PATH" }
newoption { trigger = "lua-lib-name", category = "YGOPro - lua", description = "", value = "NAME", default = LUA_LIB_NAME }

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

newoption { trigger = "no-audio", category = "YGOPro", description = "" }
newoption { trigger = "audio-lib", category = "YGOPro", description = "", value = "miniaudio, irrklang", default = AUDIO_LIB }

newoption { trigger = "miniaudio-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "miniaudio-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "no-build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
newoption { trigger = "opus-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "opus-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "vorbis-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "vorbis-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "ogg-include-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }
newoption { trigger = "ogg-lib-dir", category = "YGOPro - miniaudio", description = "", value = "PATH" }

newoption { trigger = "use-irrklang", category = "YGOPro - irrklang", description = "Deprecated, use audio-lib=irrklang" }
newoption { trigger = "no-use-irrklang", category = "YGOPro - irrklang", description = "Deprecated, use no-audio" }
newoption { trigger = "irrklang-include-dir", category = "YGOPro - irrklang", description = "", value = "PATH" }
newoption { trigger = "irrklang-lib-dir", category = "YGOPro - irrklang", description = "", value = "PATH" }

newoption { trigger = "irrklang-pro", category = "YGOPro - irrklang - pro", description = "" }
newoption { trigger = "no-irrklang-pro", category = "YGOPro - irrklang - pro", description = "" }
newoption { trigger = "irrklang-pro-release-lib-dir", category = "YGOPro - irrklang - pro", description = "", value = "PATH" }
newoption { trigger = "irrklang-pro-debug-lib-dir", category = "YGOPro - irrklang - pro", description = "", value = "PATH" }
newoption { trigger = 'build-ikpmp3', category = "YGOPro - irrklang - ikpmp3", description = "" }

newoption { trigger = "winxp-support", category = "YGOPro", description = "" }
newoption { trigger = "mac-arm", category = "YGOPro", description = "Compile for Apple Silicon Mac" }
newoption { trigger = "mac-intel", category = "YGOPro", description = "Compile for Intel Mac" }

function GetParam(param)
    return _OPTIONS[param] or os.getenv(string.upper(string.gsub(param,"-","_")))
end

if GetParam("build-lua") then
    BUILD_LUA = true
elseif GetParam("no-build-lua") then
    BUILD_LUA = false
end
if not BUILD_LUA then
    -- at most times you need to change this if you change BUILD_LUA to false
    -- make sure your lua lib is built with C++ and version >= 5.3
    LUA_LIB_NAME = GetParam("lua-lib-name")
    LUA_INCLUDE_DIR = GetParam("lua-include-dir") or os.findheader(LUA_LIB_NAME)
    LUA_LIB_DIR = GetParam("lua-lib-dir") or os.findlib(LUA_LIB_NAME)
end

if GetParam("build-event") then
    BUILD_EVENT = true
elseif GetParam("no-build-event") then
    BUILD_EVENT = false
end
if not BUILD_EVENT then
    EVENT_INCLUDE_DIR = GetParam("event-include-dir") or os.findheader("event")
    EVENT_LIB_DIR = GetParam("event-lib-dir") or os.findlib("event")
end

if GetParam("build-freetype") then
    BUILD_FREETYPE = true
elseif GetParam("no-build-freetype") then
    BUILD_FREETYPE = false
end
if not BUILD_FREETYPE then
    FREETYPE_INCLUDE_DIR = GetParam("freetype-include-dir") or os.findheader("freetype")
    FREETYPE_LIB_DIR = GetParam("freetype-lib-dir") or os.findlib("freetype")
end

if GetParam("build-sqlite") then
    BUILD_SQLITE = true
elseif GetParam("no-build-sqlite") then
    BUILD_SQLITE = false
end
if not BUILD_SQLITE then
    SQLITE_INCLUDE_DIR = GetParam("sqlite-include-dir") or os.findheader("sqlite3")
    SQLITE_LIB_DIR = GetParam("sqlite-lib-dir") or os.findlib("sqlite3")
end

if GetParam("build-irrlicht") then
    BUILD_IRRLICHT = true
elseif GetParam("no-build-irrlicht") then
    BUILD_IRRLICHT = false
end
if not BUILD_IRRLICHT then
    IRRLICHT_INCLUDE_DIR = GetParam("irrlicht-include-dir") or os.findheader("irrlicht")
    IRRLICHT_LIB_DIR = GetParam("irrlicht-lib-dir") or os.findlib("irrlicht")
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
elseif GetParam("no-use-irrklang") then
    print("Warning: --no-use-irrklang is deprecated, use --no-audio")
    USE_AUDIO = false
elseif GetParam("use-irrklang") then
    print("Warning: --use-irrklang is deprecated, use --audio-lib=irrklang")
    USE_AUDIO = true
    AUDIO_LIB = "irrklang"
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
                OPUS_INCLUDE_DIR = GetParam("opus-include-dir") or os.findheader("opus")
                OPUS_LIB_DIR = GetParam("opus-lib-dir") or os.findlib("opusfile")
                VORBIS_INCLUDE_DIR = GetParam("vorbis-include-dir") or os.findheader("vorbis")
                VORBIS_LIB_DIR = GetParam("vorbis-lib-dir") or os.findlib("vorbis")
                OGG_INCLUDE_DIR = GetParam("ogg-include-dir") or os.findheader("ogg")
                OCG_LIB_DIR = GetParam("ogg-lib-dir") or os.findlib("ogg")
            end
        end
    elseif AUDIO_LIB == "irrklang" then
        print("Warning: irrKlang is deprecated and may be removed in future, please consider switching to miniaudio")
        IRRKLANG_INCLUDE_DIR = GetParam("irrklang-include-dir") or "../irrklang/include"
        if os.istarget("windows") then
            IRRKLANG_LIB_DIR = "../irrklang/lib/Win32-visualStudio"
        elseif os.istarget("linux") then
            IRRKLANG_LIB_DIR = "../irrklang/bin/linux-gcc-64"
            IRRKLANG_LINK_RPATH = "-Wl,-rpath=./irrklang/bin/linux-gcc-64/"
        elseif os.istarget("macosx") then
            IRRKLANG_LIB_DIR = "../irrklang/bin/macosx-gcc"
        end
        IRRKLANG_LIB_DIR = GetParam("irrklang-lib-dir") or IRRKLANG_LIB_DIR
        if GetParam("irrklang-pro") and os.istarget("windows") then
            IRRKLANG_PRO = true
        elseif GetParam("no-irrklang-pro") then
            IRRKLANG_PRO = false
        end
        if IRRKLANG_PRO then
            -- irrklang pro can't use the pro lib to debug
            IRRKLANG_PRO_RELEASE_LIB_DIR = GetParam("irrklang-pro-release-lib-dir") or "../irrklang/lib/Win32-vs2019"
            IRRKLANG_PRO_DEBUG_LIB_DIR = GetParam("irrklang-pro-debug-lib-dir") or "../irrklang/lib/Win32-visualStudio-debug"
        end
        IRRKLANG_PRO_BUILD_IKPMP3 = GetParam("build-ikpmp3") or IRRKLANG_PRO
    else
        error("Unknown audio library: " .. AUDIO_LIB)
    end
end

if GetParam("winxp-support") and os.istarget("windows") then
    WINXP_SUPPORT = true
end

if os.istarget("macosx") then
    if GetParam("mac-arm") then
        MAC_ARM = true
    end
    if GetParam("mac-intel") then
        MAC_INTEL = true
    end
    
    if MAC_ARM then
        TARGET_MAC_ARM = true
    elseif not MAC_INTEL then
        -- automatic target arm64, need extra detect
        local uname = os.outputof("uname -m")
        local proctranslated = os.outputof("sysctl sysctl.proc_translated")
        if uname:find("arm") or proctranslated then
            print("Detected Apple Silicon Mac")
            TARGET_MAC_ARM = true
        end
    end
end

workspace "YGOPro"
    location "build"
    language "C++"
    objdir "obj"

    configurations { "Release", "Debug" }

    filter "system:windows"
        entrypoint "mainCRTStartup"
        systemversion "latest"
        startproject "YGOPro"
        if WINXP_SUPPORT then
            defines { "WINVER=0x0501" }
            toolset "v141_xp"
        else
            defines { "WINVER=0x0601" } -- WIN7
        end

    filter "system:macosx"
        libdirs { "/usr/local/lib" }
        if MAC_ARM then
            buildoptions { "-arch arm64" }
        end
        if MAC_INTEL then
            buildoptions { "-arch x86_64", "-mavx", "-mfma" }
        end
        if MAC_ARM and MAC_INTEL then
            architecture "universal"
        end
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
        if linktimeoptimization then
            linktimeoptimization "On"
        else
            flags { "LinkTimeOptimization" }
        end
        staticruntime "On"
        disablewarnings { "4244", "4267", "4838", "4996", "6011", "6031", "6054", "6262" }

    filter { "configurations:Release", "not action:vs*" }
        symbols "On"
        defines "NDEBUG"

    filter { "configurations:Debug", "action:vs*" }
        disablewarnings { "6011", "6031", "6054", "6262" }

    filter "action:vs*"
        cdialect "C11"
        if not WINXP_SUPPORT then
           conformancemode "On" 
        end
        vectorextensions "SSE2"
        buildoptions { "/utf-8" }
        defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "not action:vs*"
        buildoptions { "-fno-strict-aliasing", "-Wno-multichar", "-Wno-format-security" }
        if not MAC_ARM and not MAC_INTEL then
            buildoptions "-march=native"
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
    if BUILD_SQLITE then
        include "sqlite3"
    end
    if USE_AUDIO then
        if AUDIO_LIB=="miniaudio" then
            include "miniaudio"
        end
        if IRRKLANG_PRO_BUILD_IKPMP3 then
            include "ikpmp3"
        end
    end
