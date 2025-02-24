project "cminiaudio"
    kind "StaticLib"
    files { "*.c", "*.h" }

    forceincludes { "./stb_vorbis.h" }

    filter "system:linux"
        links { "dl", "pthread", "m" }
