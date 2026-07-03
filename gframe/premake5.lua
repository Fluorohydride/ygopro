project "YGOPro"
    kind "WindowedApp"
    rtti "Off"
    if USE_OPENMP then
        openmp "On"
    end

    defines { "_IRR_STATIC_LIB_" }
    files { "*.cpp", "*.h" }

    includedirs { "../ocgcore" }
    links { "ocgcore" }


    if BUILD_FREETYPE then
        -- Add custom include directory for FreeType before the default include directory
        includedirs { FREETYPE_CUSTOM_INCLUDE_DIR }
    end

    for _, dep in ipairs(DEPENDENCIES_METADATA) do
        local upper = string.upper(dep.name)
        includedirs { _G[upper .. "_INCLUDE_DIR"] }
        if _G["BUILD_" .. upper] then
            -- When building from source, the dependencies will be linked with their project names, which can't be changed via options.
            links { dep.name }
        else
            links { _G[upper .. "_LIB_NAME"] }
            libdirs { _G[upper .. "_LIB_DIR"] }
        end
    end

    if not BUILD_EVENT and not os.istarget("windows") then
        links { EVENT_PTHREADS_LIB_NAME }
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
                    links { OPUSFILE_LIB_NAME, VORBISFILE_LIB_NAME, OPUS_LIB_NAME, VORBIS_LIB_NAME, OGG_LIB_NAME }
                    libdirs { OPUSFILE_LIB_DIR, OPUS_LIB_DIR, VORBIS_LIB_DIR, OGG_LIB_DIR }
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
