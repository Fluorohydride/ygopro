project "sfAudio"
	kind "StaticLib"
	cppdialect "C++14"
	includedirs { "include", "external/headers" }
	files { "src/**" }
	if _OPTIONS["use-mpg123"] then
		defines "SFAUDIO_USE_MPG123"
		excludes { "src/SoundFileReaderMp3_minimp3.cpp" }
	else
		excludes { "src/SoundFileReaderMp3_mpg123.cpp" }
	end
	filter "system:not windows"
		links { "FLAC", "vorbisfile", "vorbis", "ogg", "openAL" }
		if _OPTIONS["use-mpg123"] then
			links { "mpg123" }
		end