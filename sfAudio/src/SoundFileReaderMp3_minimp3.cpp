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
#ifndef SFAUDIO_USE_MPG123
#define MINIMP3_IMPLEMENTATION
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>
#include "sfAudio/SoundFileReaderMp3_minimp3.hpp"
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


////////////////////////////////////////////////////////////
SoundFileReaderMp3::SoundFileReaderMp3() :
m_numSamples(0),
m_position(0)
{
    m_io.read = read_cb;
    m_io.seek = seek_cb;
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
    m_io.read_data = m_io.seek_data = &stream;

    // Init mp3 decoder
    mp3dec_ex_open_cb(&m_decoder, &m_io, MP3D_SEEK_TO_SAMPLE);
    if (!m_decoder.samples)
        return false;

    // Retrieve the music attributes
    info.channelCount = m_decoder.info.channels;
    info.sampleRate   = m_decoder.info.hz;
    info.sampleCount  = m_decoder.samples;

    m_numSamples      = info.sampleCount;
    return true;
}


////////////////////////////////////////////////////////////
void SoundFileReaderMp3::seek(uint64_t sampleOffset)
{
    m_position = std::min(sampleOffset, m_numSamples);
    mp3dec_ex_seek(&m_decoder, m_position);
}


////////////////////////////////////////////////////////////
uint64_t SoundFileReaderMp3::read(int16_t* samples, uint64_t maxCount)
{
    uint64_t toRead = std::min(maxCount, m_numSamples - m_position);
    toRead = mp3dec_ex_read(&m_decoder, samples, toRead);
    m_position += toRead;
    return toRead;
}


////////////////////////////////////////////////////////////
void SoundFileReaderMp3::close()
{
    mp3dec_ex_close(&m_decoder);
}

} // namespace priv

} // namespace sf

#endif //SFAUDIO_USE_MPG123