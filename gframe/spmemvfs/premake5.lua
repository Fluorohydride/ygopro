project "cspmemvfs"
    kind "StaticLib"
    cdialect "C11"
    files { "*.c", "*.h" }

    if BUILD_SQLITE then
        includedirs { "../../sqlite3" }
    end

    filter "not action:vs*"
    defines { "_POSIX_C_SOURCE=200809L" }
