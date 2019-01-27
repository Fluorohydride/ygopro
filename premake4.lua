newoption {
	trigger		= "no-direct3d",
	description	= "Disable directx options in irrlicht if the DirectX SDK isn't installed"
}
solution "ygo"
	location "build"
	language "C++"
	objdir "obj"

	configurations { "Debug", "DebugDLL" , "Release", "ReleaseDLL" }

	configuration "windows"
		defines { "WIN32", "_WIN32", "NOMINMAX" }

	configuration "bsd"
		includedirs "/usr/local/include"
		libdirs "/usr/local/lib"

	configuration "macosx"
		includedirs "/opt/local/include"
		libdirs "/opt/local/lib"

	configuration "vs*"
		flags "EnableSSE2"
		buildoptions "-wd4996"
		defines "_CRT_SECURE_NO_WARNINGS"

	configuration "not vs*"
		buildoptions { "-fno-strict-aliasing", "-Wno-multichar" }

	configuration {"not vs*", "windows"}
		buildoptions "-static-libgcc"

	configuration "Debug*"
		flags "Symbols"
		defines "_DEBUG"
		targetdir "bin/debug"

	configuration { "Release", "not vs*" }
		flags "Symbols"
		defines "NDEBUG"
		buildoptions "-march=native"

	configuration "Release*"
		flags "OptimizeSpeed"
		targetdir "bin/release"

	include "ocgcore"
	include "gframe"
	include "fmt"
	if os.get()=="windows" then
		include "event"
		include "freetype"
		include "irrlicht"
		include "lua"
		include "sqlite3"
	end
