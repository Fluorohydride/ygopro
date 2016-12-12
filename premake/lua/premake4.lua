project "lua"
    kind "StaticLib"

    files { "*.c", "*.h" }
    removefiles { "luac.c" }
