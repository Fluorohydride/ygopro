project "YGOPro"
    kind "WindowedApp"
    rtti "Off"
    if USE_OPENMP then
        openmp "On"
    end

    defines { "_IRR_STATIC_LIB_" }
    files { "*.cpp", "*.h" }
    includedirs { "../ocgcore", EVENT_INCLUDE_DIR, IRRLICHT_INCLUDE_DIR, JPEG_INCLUDE_DIR, ZLIB_INCLUDE_DIR, LZMA_INCLUDE_DIR, SQLITE_INCLUDE_DIR }
    links { "ocgcore" }

    -- When building from source, the dependencies will be linked with their project names, which can't be changed via options.

    if BUILD_LUA then
        links { "lua" }
    else
        links { LUA_LIB_NAME }
        libdirs { LUA_LIB_DIR }
    end

    if BUILD_EVENT then
        links { "event" }
    else
        links { EVENT_LIB_NAME, EVENT_PTHREADS_LIB_NAME }
        libdirs { EVENT_LIB_DIR }
    end

    if BUILD_IRRLICHT then
        links { "irrlicht" }
    else
        links { IRRLICHT_LIB_NAME }
        libdirs { IRRLICHT_LIB_DIR }
    end

    if BUILD_JPEG then
        links { "jpeg" }
    else
        links { JPEG_LIB_NAME }
        libdirs { JPEG_LIB_DIR }
    end

    if BUILD_PNG then
        links { "png" }
    else
        links { PNG_LIB_NAME }
        libdirs { PNG_LIB_DIR }
    end

    if BUILD_ZLIB then
        links { "zlib" }
    else
        links { ZLIB_LIB_NAME }
        libdirs { ZLIB_LIB_DIR }
    end

    if BUILD_FREETYPE then
        includedirs { FREETYPE_CUSTOM_INCLUDE_DIR, FREETYPE_INCLUDE_DIR }
        links { "freetype" }
    else
        includedirs { FREETYPE_INCLUDE_DIR }
        links { FREETYPE_LIB_NAME }
        libdirs { FREETYPE_LIB_DIR }
    end

    if BUILD_SQLITE then
        links { "sqlite" }
    else
        links { SQLITE_LIB_NAME }
        libdirs { SQLITE_LIB_DIR }
    end

    if BUILD_LZMA then
        links { "lzma" }
    else
        links { LZMA_LIB_NAME }
        libdirs { LZMA_LIB_DIR }
    end

    if USE_SIMD == "none" then
        defines { "STBIR_NO_SIMD" }
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
                    links { OPUS_LIB_NAME, OPUSFILE_LIB_NAME, VORBIS_LIB_NAME, VORBISFILE_LIB_NAME, OGG_LIB_NAME }
                    libdirs { OPUS_LIB_DIR, OPUSFILE_LIB_DIR, VORBIS_LIB_DIR, OGG_LIB_DIR }
                end
            end
        end
    end

    filter "system:windows"
        entrypoint "mainCRTStartup"
        files "ygopro.rc"
        links { "ws2_32", "iphlpapi", "winmm" }

    filter "not action:vs*"
        cppdialect "C++14"

    filter "system:macosx"
        links { "OpenGL.framework", "Cocoa.framework", "IOKit.framework", "Carbon.framework" }
        defines { "GL_SILENCE_DEPRECATION" }

    filter "system:linux"
        links { "GL", "X11", "dl", "pthread" }
        if USE_OPENMP then
            linkoptions { "-fopenmp" }
        end
