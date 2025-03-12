project "cspmemvfs"
    kind "StaticLib"
    cdialect "C11"
    files { "*.c", "*.h" }

    if BUILD_SQLITE then
        includedirs { "../../sqlite3" }
    end
