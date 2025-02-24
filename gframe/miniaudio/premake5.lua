project "cminiaudio"
    kind "StaticLib"
    files { "*.c", "*.h" }

    filter "action:vs*"
        forceincludes { "stb_vorbis.h" }

    filter "not action:vs*"
        forceincludes { "./stb_vorbis.h" }

    filter "system:linux"
        links { "dl", "pthread", "m" }
