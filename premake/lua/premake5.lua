project "lua"
    kind "StaticLib"

    files { "src/*.c", "src/*.h", "src/*.hpp" }
    removefiles { "src/lua.c", "src/luac.c" }

    filter "action:vs*"
        buildoptions { "/TP" }

    filter "not action:vs*"
        buildoptions { "-x c++" }

    filter "system:bsd"
        defines { "LUA_USE_POSIX" }

    filter "system:macosx"
        defines { "LUA_USE_MACOSX" }

    filter "system:linux"
        defines { "LUA_USE_LINUX" }
