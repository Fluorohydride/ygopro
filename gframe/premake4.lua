include "lzma"

project "ygopro"
    kind "ConsoleApp"

    files { "gframe.cpp", "config.h",
            "game.cpp", "game.h",
            "deck_manager.cpp", "deck_manager.h",
            "data_manager.cpp", "data_manager.h",
            "replay.cpp", "replay.h",
            "netserver.cpp", "netserver.h",
            "single_duel.cpp", "single_duel.h",
            "tag_duel.cpp", "tag_duel.h",
            "**.cc", "**.c"}
    excludes "lzma/**"
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "sqlite3", "lua" , "event"}

    configuration "not vs*"
        buildoptions { "-std=gnu++0x", "-fno-rtti" }
    configuration "not windows"
        includedirs { "/usr/include/lua", "/usr/include/lua5.2", "/usr/include/lua/5.2" }
        excludes { "COSOperator.*" }
        links { "event_pthreads", "dl", "pthread" }
