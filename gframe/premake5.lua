include "lzma/."

project "YGOPro"
    kind "WindowedApp"
    rtti "Off"
    if USE_OPENMP then
        openmp "On"
    end

    defines { "_IRR_STATIC_LIB_" }
    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore", EVENT_INCLUDE_DIR, IRRLICHT_INCLUDE_DIR, JPEG_INCLUDE_DIR, SQLITE_INCLUDE_DIR }
    links { "ocgcore", "clzma", LUA_LIB_NAME, "sqlite3", "irrlicht", JPEG_LIB_NAME, "freetype", "event" }

    if not BUILD_LUA then
        libdirs { LUA_LIB_DIR }
    end

    if not BUILD_EVENT then
        libdirs { EVENT_LIB_DIR }
        links { "event_pthreads" }
    end

    if not BUILD_IRRLICHT then
        libdirs { IRRLICHT_LIB_DIR }
    end

    if not BUILD_PNG_IRRLICHT then
        links { "png" }
        libdirs { PNG_LIB_DIR }
    end

    if not BUILD_JPEG then
        libdirs { JPEG_LIB_DIR }
    end

    if BUILD_FREETYPE then
        includedirs { FREETYPE_CUSTOM_INCLUDE_DIR, FREETYPE_INCLUDE_DIR }
    else
        includedirs { FREETYPE_INCLUDE_DIR }
        libdirs { FREETYPE_LIB_DIR }
    end

    if not BUILD_SQLITE then
        libdirs { SQLITE_LIB_DIR }
    end

    if USE_AUDIO then
        defines { "YGOPRO_USE_AUDIO" }
        if AUDIO_LIB == "miniaudio" then
            defines { "YGOPRO_USE_MINIAUDIO" }
            includedirs { MINIAUDIO_INCLUDE_DIR }
            links { "miniaudio" }
            if MINIAUDIO_SUPPORT_OPUS_VORBIS then
                defines { "YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS" }
                includedirs { MINIAUDIO_OPUS_INCLUDE_DIR, MINIAUDIO_VORBIS_INCLUDE_DIR }
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
        files "ygopro.rc"
        links { "ws2_32", "iphlpapi", "winmm" }
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
    filter "not action:vs*"
        cppdialect "C++14"

    filter "system:macosx"
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
        links { "GL", "X11", "dl", "pthread" }
        if USE_OPENMP then
            linkoptions { "-fopenmp" }
        end
        if USE_AUDIO and AUDIO_LIB == "irrklang" then
            links { "IrrKlang" }
            linkoptions{ IRRKLANG_LINK_RPATH }
        end
