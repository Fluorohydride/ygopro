project "miniaudio"
    kind "StaticLib"
    files { "extras/miniaudio_split/miniaudio.*" }

    if MINIAUDIO_SUPPORT_OPUS_VORBIS then
        files { "extras/decoders/libopus/*", "extras/decoders/libvorbis/*" }
        if MINIAUDIO_BUILD_OPUS_VORBIS then
            files { "external/ogg/src/**.c", "external/ogg/src/**.h" }
            files { "external/opus/src/**.c", "external/opus/src/**.h" }
            files { "external/opus/celt/*.c", "external/opus/celt/*.h" }
            files { "external/opus/silk/*.c", "external/opus/silk/*.h" }
            files { "external/opus/silk/float/*.c", "external/opus/silk/float/*.h" }
            files { "external/opusfile/src/**.c", "external/opusfile/src/**.h" }
            files { "external/vorbis/lib/**.c", "external/vorbis/lib/**.h" }

            removefiles { "external/opus/src/opus_demo.c", "external/opus/src/tone.c",
                        "external/opus/src/opus_encoder.c", 
                        "external/vorbis/lib/psy.h", "external/vorbis/lib/psytune.c", "external/vorbis/lib/tone.c",}

            defines { "USE_ALLOCA", "OPUS_BUILD" }

            includedirs {
                "external/ogg/include",
                "external/opus/include",
                "external/opus/celt",
                "external/opus/silk",
                "external/opus/silk/float",
                "external/opusfile/include",
                "external/vorbis/include"
            }
        else
            includedirs { OPUS_INCLUDE_DIR, VORBIS_INCLUDE_DIR, OGG_INCLUDE_DIR }
            links { "opusfile", "vorbisfile", "opus", "vorbis", "ogg" }
            libdirs { OPUS_LIB_DIR, VORBIS_LIB_DIR, OGG_LIBDIR }
        end
    end

    filter "system:linux"
        links { "dl", "pthread", "m" }
