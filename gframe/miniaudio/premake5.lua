project "cminiaudio"
    kind "StaticLib"
    files { "*.c", "*.h" }

    filter "system:linux"
        links { "dl", "pthread", "m" }
