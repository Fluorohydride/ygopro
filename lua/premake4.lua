project "lua"
    kind "StaticLib"

    files { "**.cc", "**.cpp", "**.c", "**.h" }
    excludes { "lua.c", "luac.c" }
    
    configuration "vs*"
        buildoptions { "/TP" }
    configuration "not vs*"
        buildoptions { "-x c++" }
