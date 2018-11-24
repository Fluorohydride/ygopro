project "fmt"
    kind "StaticLib"

    includedirs { "include" } 
    files { "src/*.cpp", "src/*.c", "src/*.cc", "src/*.cxx", "src/*.hpp", "src/*.h" }
    files { "include/**.cpp", "include/**.c", "include/**.cc", "include/**.cxx", "include/**.hpp", "include/**.h" }
