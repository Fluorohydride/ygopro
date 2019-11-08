project "Irrlicht"
	kind "StaticLib"
	includedirs "include"
	dofile("defines.lua")
	exceptionhandling "Off"
	rtti "Off"
	files { "**.cpp", "**.c", "**.cxx", "**.hpp", "**.h" }

	filter "options:no-direct3d"
		defines "NO_IRR_COMPILE_WITH_DIRECT3D_9_"

	filter "options:not no-direct3d"
		defines "IRR_COMPILE_WITH_DX9_DEV_PACK"
		includedirs "$(DXSDK_DIR)Include"
		libdirs "$(DXSDK_DIR)Lib/x86"

	filter "action:vs*"
		defines { "IRRLICHT_FAST_MATH", "UNICODE", "_UNICODE" }

	filter "system:windows"
		links "imm32"