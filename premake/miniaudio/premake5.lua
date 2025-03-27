project "miniaudio"
    kind "StaticLib"
    files { "*.c", "*.h" }

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

    includedirs { ".",
        "external/ogg/include",
        "external/opus/include",
        "external/opus/celt",
        "external/opus/silk",
        "external/opus/silk/float",
        "external/opusfile/include",
        "external/vorbis/include" }

    filter "system:linux"
        links { "dl", "pthread", "m" }
