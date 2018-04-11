include "lzma/."

project "ygopro"
    kind "ConsoleApp"
	
    defines { "YGOPRO_SERVER_MODE" }

    files { "gframe.cpp", "config.h",
            "game.cpp", "game.h",
            "deck_manager.cpp", "deck_manager.h",
            "data_manager.cpp", "data_manager.h",
            "replay.cpp", "replay.h",
            "netserver.cpp", "netserver.h",
            "single_duel.cpp", "single_duel.h",
            "tag_duel.cpp", "tag_duel.h" }
    includedirs { "../ocgcore" }
    links { "ocgcore", "clzma", "sqlite3", "lua" , "event"}

    configuration "windows"
        files "ygopro.rc"
        includedirs { "../event/include", "../sqlite3" }
        links { "ws2_32" }

    configuration "not vs*"
        buildoptions { "-std=gnu++0x", "-fno-rtti" }
    configuration "not windows"
        includedirs { "/usr/include/lua", "/usr/include/lua5.3", "/usr/include/lua/5.3" }
        links { "event_pthreads", "dl", "pthread" }
