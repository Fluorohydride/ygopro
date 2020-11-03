////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#ifdef SFAUDIO_USE_MPG123
#include <algorithm>
#include "sfAudio/SoundFileReaderMp3_mpg123.hpp"
#include "sfAudio/System/MemoryInputStream.hpp"
#include <iostream>

static size_t read_cb(void* ptr, size_t size, void* data)
{
    sf::InputStream* stream = static_cast<sf::InputStream*>(data);
    return static_cast<std::size_t>(stream->read(ptr, size));
}

static int seek_cb(uint64_t offset, void* data)
{
    sf::InputStream* stream = static_cast<sf::InputStream*>(data);
    uint64_t position = stream->seek(offset);
    return position < 0 ? -1 : 0;
}

#define HDR_GET_LAYER(h)            (((h[1]) >> 1) & 3)
#define HDR_GET_BITRATE(h)          ((h[2]) >> 4)
#define HDR_GET_SAMPLE_RATE(h)      (((h[2]) >> 2) & 3)
static int hdr_valid(const uint8_t *h) {
	return h[0] == 0xff &&
		((h[1] & 0xF0) == 0xf0 || (h[1] & 0xFE) == 0xe2) &&
		(HDR_GET_LAYER(h) != 0) &&
		(HDR_GET_BITRATE(h) != 15) &&
		(HDR_GET_SAMPLE_RATE(h) != 3);
}

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
bool SoundFileReaderMp3::check(InputStream& stream)
{
    char hdr[10];
    if (stream.read(hdr, sizeof(hdr)) < sizeof(hdr))
        return false;
    if (!memcmp(hdr, "ID3", 3) && !((hdr[5] & 15) || (hdr[6] & 0x80) || (hdr[7] & 0x80) || (hdr[8] & 0x80) || (hdr[9] & 0x80)))
        return true;
    if (hdr_valid((const uint8_t *)hdr))
        return true;
    return false;
}

// Custom reader I/O for OpenFromMemory
ssize_t MemoryDataRead(void* rawMp3Data, void* buffer, size_t nbyte);
off_t MemoryDataLSeek(void* rawMp3Data, off_t offset, int whence);
void MemoryDataCleanup(void* rawMp3Data);


////////////////////////////////////////////////////////////
SoundFileReaderMp3::SoundFileReaderMp3() :
	myHandle(nullptr),
	myBufferSize(0),
	myBuffer(nullptr),
	mySamplingRate(0)
{
	int err = MPG123_OK;
	if((err = mpg123_init()) != MPG123_OK) {
		std::cerr << mpg123_plain_strerror(err) << std::endl;
		return;
	}
	myHandle = mpg123_new(nullptr, &err);
	if(!myHandle) {
		std::cerr << "Unable to create mpg123 handle: " << mpg123_plain_strerror(err) << std::endl;
		return;
	}

	mpg123_replace_reader_handle(myHandle, &MemoryDataRead, &MemoryDataLSeek, &MemoryDataCleanup);
}


////////////////////////////////////////////////////////////
SoundFileReaderMp3::~SoundFileReaderMp3()
{
    close();
}


////////////////////////////////////////////////////////////
bool SoundFileReaderMp3::open(InputStream& stream, Info& info)
{
    // Init IO callbacks
	if(mpg123_open_handle(myHandle, &stream) != MPG123_OK) {
		std::cerr << mpg123_strerror(myHandle) << std::endl;
		return false;
	}
	long rate = 0;
	int  channels = 0, encoding = 0;
	if(mpg123_getformat(myHandle, &rate, &channels, &encoding) != MPG123_OK) {
		std::cerr << "Failed to get format information for Memory Object" << std::endl;
		return false;
	}
	myBufferSize = mpg123_outblock(myHandle);
	myBuffer = new unsigned char[myBufferSize];
	if(!myBuffer) {
		std::cerr << "Failed to reserve memory for decoding one frame for Memory Object" << std::endl;
		return false;
	}
	info.sampleRate = rate;
	info.channelCount = channels;
	mpg123_seek(myHandle, 0, SEEK_END);
	info.sampleCount = mpg123_tell(myHandle);
	mpg123_seek(myHandle, 0, SEEK_SET);
    return true;
}


////////////////////////////////////////////////////////////
void SoundFileReaderMp3::seek(uint64_t sampleOffset)
{
	if(myHandle)
		mpg123_seek(myHandle, static_cast<off_t>(sampleOffset), SEEK_SET);
}


////////////////////////////////////////////////////////////
uint64_t SoundFileReaderMp3::read(int16_t* samples, uint64_t maxCount)
{
	if(myHandle) {
		size_t done;
		mpg123_read(myHandle, (unsigned char*)samples, std::min<uint64_t>(maxCount, myBufferSize), &done);
		return done / sizeof(short);
	}
	return 0;
}


////////////////////////////////////////////////////////////
void SoundFileReaderMp3::close()
{
	if(myBuffer) {
		delete[] myBuffer;
		myBuffer = nullptr;
	}
	mpg123_close(myHandle);
	mpg123_delete(myHandle);
	mpg123_exit();
}

ssize_t MemoryDataRead(void* rawMp3Data, void* buffer, size_t nbyte) {
	sf::InputStream* stream = static_cast<sf::InputStream*>(rawMp3Data);
	return static_cast<ssize_t>(stream->read(buffer, nbyte));
}

off_t MemoryDataLSeek(void* rawMp3Data, off_t offset, int whence) {
	sf::InputStream* stream = static_cast<sf::InputStream*>(rawMp3Data);
	auto cur = stream->tell();
	switch(whence) {
		case SEEK_SET: cur = offset; break;
		case SEEK_CUR: cur += offset; break;
		case SEEK_END: cur = stream->getSize() + offset; break;
	}
	uint64_t position = stream->seek(cur);
	return stream->tell();
}

void MemoryDataCleanup(void* rawMp3Data) {
}

} // namespace priv

} // namespace sf

#endif //SFAUDIO_USE_MPG123