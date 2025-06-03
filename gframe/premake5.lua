include "lzma/."
include "spmemvfs/."

project "YGOPro"
    kind "WindowedApp"
    cppdialect "C++14"
    rtti "Off"
    openmp "On"

    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "cspmemvfs", LUA_LIB_NAME, "sqlite3", "irrlicht", "freetype", "event" }

    if not BUILD_LUA then
        libdirs { LUA_LIB_DIR }
    end

    if BUILD_EVENT then
        includedirs { "../event/include" }
    else
        includedirs { EVENT_INCLUDE_DIR }
        libdirs { EVENT_LIB_DIR }
        links { "event_pthreads" }
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
        if AUDIO_LIB == "miniaudio" then
            defines { "YGOPRO_USE_MINIAUDIO" }
            includedirs { "../miniaudio" }
            links { "miniaudio" }
            if MINIAUDIO_SUPPORT_OPUS_VORBIS then
                defines { "YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS" }
                includedirs { "../miniaudio/extras/decoders/libopus", "../miniaudio/extras/decoders/libvorbis" }
                if not MINIAUDIO_BUILD_OPUS_VORBIS then
                    links { "opusfile", "vorbisfile", "opus", "vorbis", "ogg" }
                    libdirs { OPUS_LIB_DIR, OPUSFILE_LIB_DIR, VORBIS_LIB_DIR, OGG_LIB_DIR }
                end
            end
        end
        if AUDIO_LIB == "irrklang" then
            defines { "YGOPRO_USE_IRRKLANG" }
            includedirs { IRRKLANG_INCLUDE_DIR }
            if not IRRKLANG_PRO then
                libdirs { IRRKLANG_LIB_DIR }
            end
            if IRRKLANG_PRO_BUILD_IKPMP3 then
                links { "ikpmp3" }
            end
        end
    end

    filter "system:windows"
        entrypoint "mainCRTStartup"
        defines { "_IRR_WCHAR_FILESYSTEM" }
        files "ygopro.rc"
        links { "ws2_32" }
        if USE_AUDIO and AUDIO_LIB == "irrklang" then
            links { "irrKlang" }
            if IRRKLANG_PRO then
                defines { "IRRKLANG_STATIC" }
                filter { "system:windows", "not configurations:Debug" }
                    libdirs { IRRKLANG_PRO_RELEASE_LIB_DIR }
                filter { "system:windows", "configurations:Debug" }
                    libdirs { IRRKLANG_PRO_DEBUG_LIB_DIR }
                filter {}
            end
        end

    filter "system:macosx"
        openmp "Off"
        links { "OpenGL.framework", "Cocoa.framework", "IOKit.framework" }
        defines { "GL_SILENCE_DEPRECATION" }
        if MAC_ARM then
            linkoptions { "-arch arm64" }
        end
        if MAC_INTEL then
            linkoptions { "-arch x86_64" }
        end
        if USE_AUDIO and AUDIO_LIB == "irrklang" then
            links { "irrklang" }
        end

    filter "system:linux"
        links { "GL", "X11", "Xxf86vm" }
        linkoptions { "-fopenmp" }
        if USE_AUDIO and AUDIO_LIB == "irrklang" then
            links { "IrrKlang" }
            linkoptions{ IRRKLANG_LINK_RPATH }
        end
