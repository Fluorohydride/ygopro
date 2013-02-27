project "ocgcore"
    kind "StaticLib"

    files { "**.cc", "**.cpp", "**.c", "**.h" }
    configuration "windows"
        includedirs { ".." }
    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }
    configuration "not windows"
        includedirs { "/usr/include/lua", "/usr/include/lua5.2", "/usr/include/lua/5.2" }
