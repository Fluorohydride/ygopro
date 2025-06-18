project "lua"
    kind "StaticLib"
    compileas "C++"

    files { "src/*.c", "src/*.h" }
    removefiles { "src/lua.c", "src/luac.c", "src/linit.c", "src/onelua.c" }

    filter "configurations:Debug"
        defines { "LUA_USE_APICHECK" }

    filter "system:bsd"
        defines { "LUA_USE_POSIX" }

    filter "system:macosx"
        defines { "LUA_USE_MACOSX" }

    filter "system:linux"
        defines { "LUA_USE_LINUX" }
