/*
   base64.cpp and base64.h
   base64 encoding and decoding with C++.
   More information at
	 https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
   Version: 2.rc.04 (release candidate)
   Copyright (C) 2004-2017, 2020 René Nyffenegger
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

   Templated header only version by edo9300
*/
#ifndef BASE64_H
#define BASE64_H
#include <string>
#include <vector>

static const char* base64_chars = {
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789"
	"+/"
};

static int pos_of_char(int chr) {
	//
	// Return the position of chr within base64_encode()
	//

	if(chr >= 'A' && chr <= 'Z') return chr - 'A';
	else if(chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A') + 1;
	else if(chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
	else if(chr == '+' || chr == '-') return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
	else if(chr == '/' || chr == '_') return 63; // Ditto for '/' and '_'

	return -1;
}

template<typename T = std::wstring>
T base64_encode(const uint8_t* buf, uint32_t bufLen) {
	size_t len_encoded = (bufLen + 2) / 3 * 4;
	T ret;
	ret.reserve(len_encoded);

	unsigned int pos = 0;

	while(pos < bufLen) {
		ret.push_back(static_cast<typename T::value_type>(base64_chars[(buf[pos + 0] & 0xfc) >> 2]));
		if(pos + 1 < bufLen) {
			ret.push_back(static_cast<typename T::value_type>(base64_chars[((buf[pos + 0] & 0x03) << 4) + ((buf[pos + 1] & 0xf0) >> 4)]));
			if(pos + 2 < bufLen) {
				ret.push_back(static_cast<typename T::value_type>(base64_chars[((buf[pos + 1] & 0x0f) << 2) + ((buf[pos + 2] & 0xc0) >> 6)]));
				ret.push_back(static_cast<typename T::value_type>(base64_chars[buf[pos + 2] & 0x3f]));
			} else {
				ret.push_back(static_cast<typename T::value_type>(base64_chars[(buf[pos + 1] & 0x0f) << 2]));
				ret.push_back(static_cast<typename T::value_type>('='));
			}
		} else {
			ret.push_back(static_cast<typename T::value_type>(base64_chars[(buf[pos + 0] & 0x03) << 4]));
			ret.push_back(static_cast<typename T::value_type>('='));
			ret.push_back(static_cast<typename T::value_type>('='));
		}
		pos += 3;
	}
	return ret;
}

template<typename R = std::vector<uint8_t>, typename T = wchar_t>
R base64_decode(const T* encoded_string, size_t length_of_string) {
	using valtype = typename R::value_type;
	size_t pos = 0;

	//
	// The approximate length (bytes) of the decoded string might be one ore
	// two bytes smaller, depending on the amount of trailing equal signs
	// in the encoded string. This approximation is needed to reserve
	// enough space in the string to be returned.
	//
	size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
	R ret;
	ret.reserve(approx_length_of_decoded_string);

	while(pos < length_of_string) {
		auto pos_of_char_0 = pos_of_char(encoded_string[pos + 0]);
		auto pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);
		if(pos_of_char_0 == -1 || pos_of_char_1 == -1)
			break;
		ret.push_back(static_cast<valtype>((pos_of_char_0 << 2) + ((pos_of_char_1 & 0x30) >> 4)));

		if(static_cast<char>(encoded_string[pos + 2]) != '=' && static_cast<char>(encoded_string[pos + 2]) != '.') { // accept URL-safe base 64 strings, too, so check for '.' also.

			auto pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
			if(pos_of_char_2 == -1)
				break;
			ret.push_back(static_cast<valtype>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

			if(static_cast<char>(encoded_string[pos + 3]) != '=' && static_cast<char>(encoded_string[pos + 3]) != '.') {
				auto pos_of_char_3 = pos_of_char(encoded_string[pos + 3]);
				if(pos_of_char_3 == -1)
					break;
				ret.push_back(static_cast<valtype>(((pos_of_char_2 & 0x03) << 6) + pos_of_char_3));
			}
		}
		pos += 4;
	}

	return ret;
}

template<typename R = std::vector<uint8_t>, typename T = std::wstring>
R base64_decode(const T& encoded_string) {
	return base64_decode<R, T::value_type>(encoded_string.data(), encoded_string.size());
}
#endif //BASE64_H