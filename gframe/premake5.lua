include "lzma/."
include "spmemvfs/."

project "YGOPro"
    kind "WindowedApp"
    cppdialect "C++14"
    rtti "Off"

    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "cspmemvfs", LUA_LIB_NAME, "sqlite3", "irrlicht", "freetype", "event" }

    if BUILD_EVENT then
        includedirs { "../event/include" }
    else
        includedirs { EVENT_INCLUDE_DIR }
        libdirs { EVENT_LIB_DIR }
    end

    if BUILD_IRRLICHT then
        includedirs { "../irrlicht/include" }
    else
        includedirs { IRRLICHT_INCLUDE_DIR }
        libdirs { IRRLICHT_LIB_DIR }
    end

    if BUILD_FREETYPE then
        includedirs { "../freetype/include" }
    else
        includedirs { FREETYPE_INCLUDE_DIR }
        libdirs { FREETYPE_LIB_DIR }
    end

    if BUILD_SQLITE then
        includedirs { "../sqlite3" }
    else
        includedirs { SQLITE_INCLUDE_DIR }
        libdirs { SQLITE_LIB_DIR }
    end

    if USE_AUDIO then
        defines { "YGOPRO_USE_AUDIO" }
        includedirs { "../miniaudio" }
        links { "miniaudio" }
        if not BUILD_OPUS_VORBIS and MINIAUDIO_SUPPORT_OPUS_VORBIS then
            links { "opusfile", "vorbisfile" }
        end
    end

    filter "system:windows"
        defines { "_IRR_WCHAR_FILESYSTEM" }
        files "ygopro.rc"
        libdirs { "$(DXSDK_DIR)Lib/x86" }
        links { "opengl32", "ws2_32", "winmm", "gdi32", "kernel32", "user32", "imm32" }
    filter "not system:windows"
        links { "event_pthreads", "dl", "pthread" }
    filter "system:macosx"
        links { "z" }
        defines { "GL_SILENCE_DEPRECATION" }
        if MAC_ARM then
            buildoptions { "--target=arm64-apple-macos12" }
            linkoptions { "-arch arm64" }
        end
    filter "system:linux"
        links { "GL", "X11", "Xxf86vm" }
