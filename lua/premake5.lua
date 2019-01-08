project "lua"
	kind "StaticLib"

	files { "**.cc", "**.cpp", "**.c", "**.h" }
	excludes { "lua.c", "luac.c" }

	filter "action:vs*"
		buildoptions { "/TP" }
	filter "action:not vs*"
		buildoptions { "-x c++" }