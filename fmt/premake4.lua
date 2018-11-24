project "fmt"
    kind "StaticLib"

    includedirs { "include" } 
    files { "**.cpp", "**.c", "**.cc", "**.cxx", "**.hpp", "**.h" }
