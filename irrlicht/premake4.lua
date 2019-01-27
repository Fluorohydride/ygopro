project "Irrlicht"
	kind "StaticLib"

	includedirs { "include", "src/zlib", "src/jpeglib", "src/libpng", "$(DXSDK_DIR)Include" } 
	libdirs "$(DXSDK_DIR)Lib/x86"
	dofile("../irrlicht defines.lua")
	flags { "NoExceptions", "NoRTTI" }
	files { "**.cpp", "**.c", "**.cxx", "**.hpp", "**.h" }
	configuration "no-direct3d"
		defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"
	configuration "vs*"
		defines { "IRRLICHT_FAST_MATH", "UNICODE", "_UNICODE" }
	configuration "windows"
		links "imm32"
