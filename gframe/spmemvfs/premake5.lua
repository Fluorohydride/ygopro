project "cspmemvfs"
    kind "StaticLib"
    files { "*.c", "*.h" }

    if BUILD_SQLITE then
        includedirs { "../../sqlite" }
    end
