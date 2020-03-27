/*
   base64.cpp and base64.h
   base64 encoding and decoding with C++.
   Version: 1.01.00
   Copyright (C) 2004-2017 René Nyffenegger
   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this source code must not be misrepresented; you must not
	  claim that you wrote the original source code. If you use this source code
	  in a product, an acknowledgment in the product documentation would be
	  appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
	  misrepresented as being the original source code.
   3. This notice may not be removed or altered from any source distribution.
   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/
#ifndef BASE64_H
#define BASE64_H
#include <string>
#include <vector>

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static const auto base64_charsb = base64_chars.data();

template<typename T = std::wstring>
T base64_encode(const uint8_t* buf, unsigned int bufLen) {
	T ret;
	ret.reserve(((4 * 4 / 3) + 3) & ~3);
	int i = 0;
	int j = 0;
	uint8_t char_array_3[3];
	uint8_t char_array_4[4];

	while(bufLen--) {
		char_array_3[i++] = *(buf++);
		if(i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i < 4); i++)
				ret += static_cast<typename T::value_type>(base64_charsb[char_array_4[i]]);
			i = 0;
		}
	}

	if(i) {
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for(j = 0; (j < i + 1); j++)
			ret += static_cast<typename T::value_type>(base64_charsb[char_array_4[j]]);

		while((i++ < 3))
			ret += static_cast<typename T::value_type>('=');
	}
	return ret;
}
template<typename T = wchar_t>
std::vector<uint8_t> base64_decode(const T* encoded_string, size_t in_len) {
	static auto is_base64 = [](uint8_t c) -> bool {
		return (isalnum(c) || (c == '+') || (c == '/'));
	};
	int i = 0;
	int j = 0;
	int in_ = 0;
	uint8_t char_array_4[4], char_array_3[3];
	std::vector<uint8_t> ret;

	while(in_len-- && (static_cast<char>(encoded_string[in_]) != '=') && is_base64(static_cast<char>(encoded_string[in_]))) {
		char_array_4[i++] = static_cast<char>(encoded_string[in_]); in_++;
		if(i == 4) {
			for(i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(static_cast<char>(char_array_4[i])) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for(i = 0; (i < 3); i++)
				ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if(i) {
		for(j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(static_cast<char>(char_array_4[j])) & 0xff;

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for(j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}
#endif //BASE64_H