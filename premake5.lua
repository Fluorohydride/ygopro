workspace "ygo"
	location "build"
	language "C++"
	objdir "obj"
	startproject "ygopro"

	configurations { "Debug", "DebugDLL" , "Release", "ReleaseDLL" }
	defines "LUA_COMPAT_5_2"

	filter "system:windows"
		defines { "WIN32", "_WIN32", "NOMINMAX" }

	filter "system:bsd"
		defines "LUA_USE_POSIX"
		includedirs "/usr/local/include"
		libdirs "/usr/local/lib"

	filter "system:macosx"
		defines "LUA_USE_MACOSX"
		includedirs "/opt/local/include"
		libdirs "/opt/local/lib"

	filter "system:linux"
		defines { "LUA_USE_LINUX" }

	filter "action:vs*"
		vectorextensions "SSE2"
		buildoptions "-wd4996"
		defines "_CRT_SECURE_NO_WARNINGS"

	filter "action:not vs*"
		buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

	filter { "action:not vs*", "system:windows" }
	  buildoptions { "-static-libgcc" }

	filter "configurations:Debug*"
		symbols "On"
		defines "_DEBUG"
		targetdir "bin/debug"
		
	filter { "configurations:Release*" , "action:not vs*" }
		symbols "On"
		defines "NDEBUG"
		buildoptions "-march=native"

	filter "configurations:Release*"
		optimize "Speed"
		targetdir "bin/release"

	filter "configurations:*DLL"
		defines "YGOPRO_BUILD_DLL"
	filter{}
	include "ocgcore"
	include "gframe"
	include "fmt"
	if os.ishost("windows") then
		include "event"
		include "freetype"
		include "irrlicht"
		include "lua"
		include "sqlite3"
	end
