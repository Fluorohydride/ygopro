newoption {
	trigger		= "no-direct3d",
	description	= "Disable directx options in irrlicht if the DirectX SDK isn't installed"
}
workspace "ygo"
	location "build"
	language "C++"
	objdir "obj"
	startproject "ygopro"

	configurations { "Debug", "DebugDLL" , "Release", "ReleaseDLL" }

	filter "system:windows"
		defines { "WIN32", "_WIN32", "NOMINMAX" }

	filter "system:bsd"
		includedirs "/usr/local/include"
		libdirs "/usr/local/lib"

	filter "system:macosx"
		includedirs "/opt/local/include"
		libdirs "/opt/local/lib"

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

	include "ocgcore"
	include "gframe"
	include "fmt"
	if os.istarget("windows") then
		include "event"
		include "freetype"
		include "irrlicht"
		include "lua"
		include "sqlite3"
	end
