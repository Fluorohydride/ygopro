project "ocgcore"
    kind "StaticLib"

    files { "**.cc", "**.cpp", "**.c", "**.h" }
    configuration "not vs*"
        buildoptions { "-std=gnu++0x" }
