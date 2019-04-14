project "cspmemvfs"
    kind "StaticLib"
    files { "**.c", "**.h" }

    configuration "windows"
        includedirs { "../../sqlite3" }
