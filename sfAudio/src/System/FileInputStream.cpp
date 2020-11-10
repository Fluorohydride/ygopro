////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2016 Laurent Gomila (laurent@sfml-dev.org)
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
#ifdef _WIN32
#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <sfAudio/System/FileInputStream.hpp>

namespace sf
{
////////////////////////////////////////////////////////////
FileInputStream::FileInputStream()
    : m_file(NULL)
{

}


////////////////////////////////////////////////////////////
FileInputStream::~FileInputStream()
{
    if (m_file)
        std::fclose(m_file);
}


////////////////////////////////////////////////////////////
bool FileInputStream::open(const std::string &filename)
{
    if (m_file)
        std::fclose(m_file);

#if defined(UNICODE) && defined(_WIN32)
	auto len = MultiByteToWideChar(CP_UTF8, 0, filename.data(), -1, nullptr, 0);
	wchar_t* str = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, filename.data(), -1, str, len);
	m_file = _wfopen(str, L"rb");
	delete[] str;
#else
    m_file = std::fopen(filename.c_str(), "rb");
#endif

    return m_file != NULL;
}


////////////////////////////////////////////////////////////
uint64_t FileInputStream::read(void *data, uint64_t size)
{
    if (m_file)
        return std::fread(data, 1, static_cast<std::size_t>(size), m_file);
    else
        return -1;
}


////////////////////////////////////////////////////////////
uint64_t FileInputStream::seek(uint64_t position)
{
    if (m_file) {
        if (std::fseek(m_file, static_cast<std::size_t>(position), SEEK_SET))
            return -1;

        return tell();
    }
    else {
        return -1;
    }
}


////////////////////////////////////////////////////////////
uint64_t FileInputStream::tell()
{
    if (m_file)
        return std::ftell(m_file);
    else
        return -1;
}


////////////////////////////////////////////////////////////
uint64_t FileInputStream::getSize()
{
    if (m_file) {
        uint64_t position = tell();
        std::fseek(m_file, 0, SEEK_END);
        uint64_t size = tell();
        seek(position);
        return size;
    }
    else {
        return -1;
    }
}

} // namespace sf
