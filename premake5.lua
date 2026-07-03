----- YGOPro build configuration script using Premake5

--- Supported systems: Windows, Linux, macOS

-- Windows (Visual Studio) build supports x86, x86_64, and ARM64, with cross-compilation support.
-- Linux build supports x86_64 and ARM64.
-- macOS build supports x86_64 and ARM64, with cross-compilation support.

---- Global settings

--- Use global variables to share settings across different scripts.

USE_DXSDK = true

USE_AUDIO = true
AUDIO_LIB = "miniaudio" -- only miniaudio is supported for now

-- Available: none, sse2, avx2, neon, best
-- "best" means avx2 on x86-* and neon on ARM
USE_SIMD = "best"

-- os.hostarch() actually returns the architecture of Premake5, and the official Windows build of Premake5 is 32-bit,
-- so we can only distinguish between AARCH64 and x86, and must use the ARM build of Premake5 on ARM platforms.
PREMAKE_ARCH = os.hostarch()

-- Return val if it's not nil; otherwise, return default.
local function ifnil(val, default)
    if val ~= nil then
        return val
    else
        return default
    end
end

---- Dependency settings

--- When building dependencies from source, the corresponding source code must be placed in the corresponding location
--- in the project folder (local), with the folder name fixed as the dependency name and the folder structure fixed as
--- the official source package extraction.
---
--- Some dependencies need extra configuration (run configure scripts, copy or rename files), see the documentation or
--- the CI workflow script for details.

-- On Windows, build all dependencies from source by default.
-- On Linux/macOS, most dependencies should be installed via the package manager.
BUILD_ALL_FROM_SOURCE = os.istarget("windows")

-- Build Lua from source by default:
-- Most package managers provide Lua compiled as C, but ocgcore requires Lua compiled as C++.
-- Some package managers do provide lua-c++ variants (e.g. liblua5.4-c++.so), which can be specified manually.
BUILD_LUA = true

-- Modified Irrlicht is required; the official version from package managers lacks proper CJK support
-- (clipboard and IME). Also, Irrlicht's bundled jpeglib/libpng/zlib/lzma are not used here.
BUILD_IRRLICHT = true

-- miniaudio is always built from source (originally a header-only library, now an independent subproject).
-- When building Opus/Vorbis from source, they are integrated directly into the miniaudio subproject.
-- To simplify the build process, support for Ogg format audio (Opus/Vorbis) is optional.
MINIAUDIO_SUPPORT_OPUS_VORBIS = true
MINIAUDIO_INCLUDE_DIR = path.getabsolute("./miniaudio")
MINIAUDIO_OPUS_INCLUDE_DIR = path.getabsolute("./miniaudio/extras/decoders/libopus")
MINIAUDIO_VORBIS_INCLUDE_DIR = path.getabsolute("./miniaudio/extras/decoders/libvorbis")

-- When building freetype, a custom include dir is prepended to prioritize custom header files.
FREETYPE_CUSTOM_INCLUDE_DIR = path.getabsolute("./freetype/custom")

-- When building libevent from source, event_pthreads is integrated into the event subproject.
-- When not building from source, both "event" and "event_pthreads" need to be linked.
EVENT_PTHREADS_LIB_NAME = "event_pthreads"

-- When building Vorbis from source, vorbisfile is integrated into the miniaudio subproject.
-- When not building from source, both "vorbis" and "vorbisfile" need to be linked.
VORBISFILE_LIB_NAME = "vorbisfile"

--- Dependency metadata entries are used to generate global variables such as LUA_INCLUDE_DIR, LUA_LIB_NAME, LUA_LIB_DIR, etc. during processing.

-- Fields:
--   name (will resolve to global variable prefix)
--   prebuilt_header (for finding directory)
--   prebuilt_header_subdir (for FindHeaderWithSubDir)
--   prebuilt_libname (for prebuilt dependencies only; default: same as name)
--   source_dir (when building dependency from source, its code should be in this directory relative to the project root; default: ./name)
--   source_header_subdir (dependency's header subdirectory relative to source_dir; default: .)
DEPENDENCIES_METADATA = {
    {
        name = "lua",
        prebuilt_header = "lua.h",
        source_header_subdir = "src",
    },
    {
        name = "event",
        prebuilt_header = "event2/event.h",
        source_header_subdir = "include",
    },
    {
        name = "freetype",
        prebuilt_header = "freetype2/ft2build.h",
        prebuilt_header_subdir = "freetype2",
        source_header_subdir = "include",
    },
    {
        name = "sqlite",
        prebuilt_header = "sqlite3.h",
        prebuilt_libname = "sqlite3",
        source_dir = "sqlite3",
    },
    {
        name = "irrlicht",
        prebuilt_header = "irrlicht.h",
        source_header_subdir = "include",
    },
    {
        name = "jpeg",
        prebuilt_header = "jpeglib.h",
        source_header_subdir = "src",
    },
    {
        name = "png",
        prebuilt_header = "png.h",
    },
    {
        name = "lzma",
        prebuilt_header = "lzma.h",
        source_header_subdir = "src/liblzma/api",
    },
    {
        name = "zlib",
        prebuilt_header = "zlib.h",
        prebuilt_libname = "z",
    },
}

-- These dependencies do not have separate [no-]build-* options; instead, they use [no-]build-opus-vorbis as general build option.
-- When building from source, they are integrated into the miniaudio subproject instead of being maintained as separate subprojects,
-- and their locations are predefined in the miniaudio subproject (./miniaudio/external/*), so those options are ignored in this case.
MINIAUDIO_DEPENDENCIES_METADATA = {
    {
        name = "opus",
        prebuilt_header = "opus/opus.h",
        prebuilt_header_subdir = "opus",
    },
    {
        name = "opusfile",
        prebuilt_header = "opus/opusfile.h",
        prebuilt_header_subdir = "opus",
    },
    {
        name = "vorbis",
        prebuilt_header = "vorbis/vorbisfile.h",
    },
    {
        name = "ogg",
        prebuilt_header = "ogg/ogg.h",
    },
}

---- Register options

--- The *-include-dir, *-lib-dir, and *-lib-name options are only used for prebuilt dependencies.
--- These options are ignored when building from source.
---
--- For *-lib-name option: Most users don't need to set it, as the script already provides conventional values.
--- The only known case where setting it is necessary is when using prebuilt lua-c++, where the lib name must be specified.

--- Platform-specific notes:
---
--- Windows: Prebuilt support is incomplete (static lib, dynamic lib, debug-specific lib to be refined).
---
--- Linux: The script already attempts to automatically locate include and lib paths for prebuilt dependencies.
--- In most cases, you should not need to manually specify parameters.
--- If a package is not found, please specify it manually. In this case, if you installed it from a well-known package manager,
--- please consider reporting the issue.
---
--- macOS: When using Homebrew, use `DYLD_LIBRARY_PATH=$(brew --prefix)/lib` to ensure Homebrew installation paths are found.
--- Note: macOS/Xcode already provides "system" versions of sqlite and zlib; Homebrew treats those packages as "keg-only"
--- and won't install them to common directories. You must manually specify paths to use Homebrew-installed versions.

--- Parameters are read from premake options (priority) and environment variables.
--- Environment variable names are uppercase versions with hyphens replaced by underscores.
---
--- Note on default values: Default values should be defined at the top of the script, not as premake option defaults;
--- otherwise, the premake option default will always take priority over environment variables.

for _, dep in ipairs(DEPENDENCIES_METADATA) do
    local name  = dep.name
    local cat   = "YGOPro - " .. name
    local build = ifnil(_G["BUILD_" .. string.upper(name)], BUILD_ALL_FROM_SOURCE)
    newoption { trigger = "build-"    .. name,    category = cat, description = "Build " .. name .. " from source; default: " .. tostring(build) }
    newoption { trigger = "no-build-" .. name,    category = cat, description = "" }
    newoption { trigger = name .. "-include-dir", category = cat, description = "", value = "PATH" }
    newoption { trigger = name .. "-lib-dir",     category = cat, description = "", value = "PATH" }
    newoption { trigger = name .. "-lib-name",    category = cat, description = "", value = "NAME" }
end
for _, dep in ipairs(MINIAUDIO_DEPENDENCIES_METADATA) do
    local name = dep.name
    local cat  = "YGOPro - miniaudio"
    newoption { trigger = name .. "-include-dir", category = cat, description = "", value = "PATH" }
    newoption { trigger = name .. "-lib-dir",     category = cat, description = "", value = "PATH" }
    newoption { trigger = name .. "-lib-name",    category = cat, description = "", value = "NAME" }
end

newoption { trigger = "build-all", category = "YGOPro", description = "Build all dependencies from source" }

newoption { trigger = "no-dxsdk", category = "YGOPro - irrlicht", description = "Do not use DirectX SDK, disable D3D9 support" }

newoption { trigger = "no-audio", category = "YGOPro", description = "Disable audio support" }
newoption { trigger = "audio-lib", category = "YGOPro", description = "Specify audio library (only miniaudio is supported for now)", value = "NAME" }

newoption { trigger = "miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "Enable support for OGG format (Opus and Vorbis) in miniaudio" }
newoption { trigger = "no-miniaudio-support-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
do
    local build_opus_vorbis = ifnil(_G["BUILD_OPUS_VORBIS"], BUILD_ALL_FROM_SOURCE)
    newoption { trigger = "build-opus-vorbis", category = "YGOPro - miniaudio", description = "Build Opus and Vorbis libraries from source; default: " .. tostring(build_opus_vorbis) }
    newoption { trigger = "no-build-opus-vorbis", category = "YGOPro - miniaudio", description = "" }
end

newoption { trigger = "vs2026-win7-support", category = "YGOPro", description = "Enable Windows 7 support (toolset v143) for Visual Studio 2026" }

newoption { trigger = "mac-arm", category = "YGOPro", description = "Cross Compile for Apple Silicon Mac" }
newoption { trigger = "mac-intel", category = "YGOPro", description = "Cross Compile for Intel Mac" }

newoption { trigger = "use-openmp", category = "YGOPro", description = "Enable OpenMP support for card picture resizing (only for benchmarking)" }

newoption { trigger = "use-simd", category = "YGOPro", description = "Specify SIMD instruction set", allowed = {
    { "none", "Turn off extra SIMD support" },
    { "sse2", "Use SSE2 instructions" },
    { "avx2", "Use AVX2 instructions" },
    { "neon", "Use NEON instructions" },
    { "best", "Default, use the best SIMD instructions (AVX2 on x86-*, NEON on ARM)" },
}}

---- Process options

-- Read settings from command line or environment variables.
-- Command-line options take priority over environment variables.
local function GetParam(param)
    return ifnil(_OPTIONS[param], os.getenv(string.upper(string.gsub(param,"-","_"))))
end

local function FindHeaderWithSubDir(header, subdir)
    local result = os.findheader(header)
    if result and subdir then
        result = path.join(result, subdir)
    end
    return result
end

local function ResolveDirectoryVariableToFullPath(varname)
    local dir = _G[varname]
    if not dir or dir == "" then
        print("::warning:: " .. varname .. " is not set")
        return
    elseif not os.isdir(dir) then
        print("::warning:: " .. varname .. " is not a valid directory: " .. dir)
        return
    end
    _G[varname] = path.getabsolute(dir)
end

-- Set dependency directories from command line or environment variables, and check their validity.
local function ResolvePreBuiltDependencyDirectory(dep)
    local upper = string.upper(dep.name)
    local include_dir_var = upper .. "_INCLUDE_DIR"
    local lib_name_var = upper .. "_LIB_NAME"
    local lib_dir_var = upper .. "_LIB_DIR"
    _G[include_dir_var] = GetParam(dep.name .. "-include-dir") or FindHeaderWithSubDir(dep.prebuilt_header, dep.prebuilt_header_subdir)
    _G[lib_name_var] = GetParam(dep.name .. "-lib-name") or dep.prebuilt_libname or dep.name
    _G[lib_dir_var] = GetParam(dep.name .. "-lib-dir") or os.findlib(_G[lib_name_var])
    ResolveDirectoryVariableToFullPath(include_dir_var)
    ResolveDirectoryVariableToFullPath(lib_dir_var)
end

-- Set the include directory for a dependency being built from source, and validate its path.
local function ResolveBuildFromSourceDependencyDirectory(dep)
    local upper = string.upper(dep.name)
    local include_dir_var = upper .. "_INCLUDE_DIR"
    local source_dir = dep.source_dir or ("./" .. dep.name)
    local source_header_subdir = dep.source_header_subdir or "."
    _G[include_dir_var] = path.join(source_dir, source_header_subdir)
    ResolveDirectoryVariableToFullPath(include_dir_var)
end

if GetParam("build-all") then
    BUILD_ALL_FROM_SOURCE = true
end

-- Process build flags and external directory settings for all library dependencies.
for _, dep in ipairs(DEPENDENCIES_METADATA) do
    local name  = dep.name
    local upper = string.upper(name)
    local flag  = "BUILD_" .. upper
    local build = ifnil(_G[flag], BUILD_ALL_FROM_SOURCE)
    if GetParam("no-build-" .. name) then
        build = false
    elseif GetParam("build-" .. name) then
        build = true
    end
    _G[flag] = build
    if build then
        ResolveBuildFromSourceDependencyDirectory(dep)
    else
        ResolvePreBuiltDependencyDirectory(dep)
    end
end

if GetParam("no-dxsdk") then
    USE_DXSDK = false
end
if USE_DXSDK and os.istarget("windows") then
    if not os.getenv("DXSDK_DIR") then
        print("::warning:: DXSDK_DIR environment variable not set, it seems you don't have the DirectX SDK installed. DirectX mode will be disabled.")
        USE_DXSDK = false
    end
end

if GetParam("no-audio") then
    USE_AUDIO = false
end

if USE_AUDIO then
    AUDIO_LIB = GetParam("audio-lib") or AUDIO_LIB
    if AUDIO_LIB == "miniaudio" then
        if GetParam("no-miniaudio-support-opus-vorbis") then
            MINIAUDIO_SUPPORT_OPUS_VORBIS = false
        elseif GetParam("miniaudio-support-opus-vorbis") then
            MINIAUDIO_SUPPORT_OPUS_VORBIS = true
        end
        if MINIAUDIO_SUPPORT_OPUS_VORBIS then
            MINIAUDIO_BUILD_OPUS_VORBIS = BUILD_ALL_FROM_SOURCE
            if GetParam("no-build-opus-vorbis") then
                MINIAUDIO_BUILD_OPUS_VORBIS = false
            elseif GetParam("build-opus-vorbis") then
                MINIAUDIO_BUILD_OPUS_VORBIS = true
            end
            if MINIAUDIO_BUILD_OPUS_VORBIS then
                -- Opus, Vorbis and Ogg dependencies are integrated into the miniaudio subproject instead of being maintained as separate subprojects.
                -- Since their locations are predefined in the miniaudio subproject, nothing needs to be done here.
            else
                for _, dep in ipairs(MINIAUDIO_DEPENDENCIES_METADATA) do
                    ResolvePreBuiltDependencyDirectory(dep)
                end
            end
        end
    else
        error("Unknown audio library: " .. AUDIO_LIB)
    end
end

USE_SIMD = GetParam("use-simd") or USE_SIMD

-- Variables indicating the target Mac architecture for cross-compilation; automatically detected based on Premake's
-- host architecture if neither --mac-arm nor --mac-intel is specified.
local mac_arm = false
local mac_intel = false

if os.istarget("macosx") then
    if GetParam("mac-arm") then
        mac_arm = true
    end
    if GetParam("mac-intel") then
        mac_intel = true
    end
end

if not mac_arm and not mac_intel and table.indexof({ "x86", "x86_64", "ARM64" }, PREMAKE_ARCH) == nil then
    print("::warning:: Detected architecture '" .. PREMAKE_ARCH .. "' is not recognized. Proceeding with the build; SIMD will be disabled.")
    USE_SIMD = "none"
end

-- Normalize "avx2" and "neon" to "best": downstream code only checks "best",
-- which already maps to AVX2 on x86-* and NEON on ARM.
if USE_SIMD == "avx2" or USE_SIMD == "neon" then
    USE_SIMD = "best"
end

if os.istarget("windows") and GetParam("vs2026-win7-support") then
    WIN7_SUPPORT = true
end

if GetParam("use-openmp") then
    USE_OPENMP = true
    if os.istarget("macosx") then
        print("::warning:: OpenMP is not supported on Clang provided by Xcode.")
    end
end

---- Premake workspace and project configuration

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
        if mac_arm and mac_intel then
            print("::warning:: Universal binary is no longer supported. Please choose either --mac-arm or --mac-intel and combine the binaries with lipo manually.")
            mac_arm = false
            mac_intel = false
        end
        if not mac_arm and not mac_intel then
            if PREMAKE_ARCH == "ARM64" then
                mac_arm = true
            else
                mac_intel = true
            end
        end
        -- We need to specify architecture on macOS to make the premake filters work correctly.
        if mac_arm then
            architecture "AARCH64"
        end
        if mac_intel then
            architecture "x86_64"
        end

    -- We need to specify architecture on Linux to make the premake filters work correctly.
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
    for _, dep in ipairs(DEPENDENCIES_METADATA) do
        if _G["BUILD_" .. string.upper(dep.name)] then
            -- Build dependency as subproject, using our pre-provided premake script (copy from the premake directory of the project before running premake)
            include((dep.source_dir or dep.name) .. "/.")
        end
    end
    if USE_AUDIO then
        if AUDIO_LIB == "miniaudio" then
            include "miniaudio/."
        end
    end
