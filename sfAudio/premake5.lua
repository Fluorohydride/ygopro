project "sfAudio"
	kind "StaticLib"
	includedirs { "include", "external/headers" }
	files { "src/**" }
	filter "system:not windows"
		links { "FLAC", "vorbisfile", "vorbis", "ogg", "openAL" }