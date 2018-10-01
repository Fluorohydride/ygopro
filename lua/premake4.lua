project "lua"
    kind "StaticLib"

    files { "**.cc", "**.cpp", "**.c", "**.h" }
    excludes { "lua.c", "luac.c" }
