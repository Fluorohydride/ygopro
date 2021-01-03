/*
   base64.h
   base64 encoding and decoding with C++.

   Header only version by edo9300


   More information at
	 https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
   Version: 2.rc.04 (release candidate)
   Copyright (C) 2004-2017, 2020 René Nyffenegger
   Copyright (C) 2021 Edoardo Lolletti
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
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1)
#include <string_view>
#else
#include "nonstd/string_view.hpp"
#endif
#include <string>
#include <algorithm>
namespace B64 {
	template<typename T>
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1)
	using basic_string_view = std::basic_string_view<T>;
#else
	using basic_string_view = nonstd::basic_string_view<T>;
#endif
};

static const char* base64_chars[2] = {
			 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			 "abcdefghijklmnopqrstuvwxyz"
			 "0123456789"
			 "+/",

			 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			 "abcdefghijklmnopqrstuvwxyz"
			 "0123456789"
			 "-_" };

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
static T insert_linebreaks(T str, size_t distance) {
	using valtype = typename T::value_type;
	//
	// Provided by https://github.com/JomaCorpFX, adapted by me.
	//
	if(str.empty())
		return str;

	size_t pos = distance;

	while(pos < str.size()) {
		str.insert(str.begin() + pos, static_cast<valtype>('\n'));
		pos += distance + 1;
	}

	return str;
}

template<typename R = std::wstring, typename T = std::string, typename valtype = typename T::value_type>
R base64_encode(const T& encoded_string, bool url = false);

template<typename T = std::wstring, typename B = char, typename S>
T base64_encode(const B* buf, S bufLen, bool url = false) {
	using valtype = typename T::value_type;
	size_t len_encoded = (bufLen + 2) / 3 * 4;

	auto trailing_char = static_cast<valtype>(url ? '.' : '=');

	//
	// Choose set of base64 characters. They differ
	// for the last two positions, depending on the url
	// parameter.
	// A bool (as is the parameter url) is guaranteed
	// to evaluate to either 0 or 1 in C++ therfore,
	// the correct character set is chosen by subscripting
	// base64_chars with url.
	//
	const char* base64_chars_ = base64_chars[url];

	T ret;
	ret.reserve(len_encoded);

	unsigned int pos = 0;

	while(pos < bufLen) {
		ret.push_back(static_cast<valtype>(base64_chars_[(buf[pos + 0] & 0xfc) >> 2]));
		if(pos + 1 < bufLen) {
			ret.push_back(static_cast<valtype>(base64_chars_[((buf[pos + 0] & 0x03) << 4) + ((buf[pos + 1] & 0xf0) >> 4)]));
			if(pos + 2 < bufLen) {
				ret.push_back(static_cast<valtype>(base64_chars_[((buf[pos + 1] & 0x0f) << 2) + ((buf[pos + 2] & 0xc0) >> 6)]));
				ret.push_back(static_cast<valtype>(base64_chars_[buf[pos + 2] & 0x3f]));
			} else {
				ret.push_back(static_cast<valtype>(base64_chars_[(buf[pos + 1] & 0x0f) << 2]));
				ret.push_back(trailing_char);
			}
		} else {
			ret.push_back(static_cast<valtype>(base64_chars_[(buf[pos + 0] & 0x03) << 4]));
			ret.push_back(trailing_char);
			ret.push_back(trailing_char);
		}
		pos += 3;
	}
	return ret;
}

template<typename R, typename T, typename valtype>
R base64_encode(const T& encoded_string, bool url) {
	return base64_encode<R, valtype>(encoded_string.data(), encoded_string.size(), url);
}

template<typename R = std::string, typename T = wchar_t>
R base64_encode(const T* encoded_string, bool url = false) {
	B64::basic_string_view<T> tmp{ encoded_string };
	return base64_encode<R, T>(tmp.data(), tmp.size(), url);
}

template <typename R = std::string, unsigned int line_length, typename T = std::string>
R encode_with_line_breaks(const T& s) {
	return insert_linebreaks(base64_encode<R>(s, false), line_length);
}

template <typename R = std::string, typename T = std::string>
R base64_encode_pem(const T& s) {
	return encode_with_line_breaks<R, 64>(s);
}

template <typename R = std::string, typename T = std::string>
R base64_encode_mime(const T& s) {
	return encode_with_line_breaks<R, 76>(s);
}

template<typename R = std::vector<uint8_t>, typename T = wchar_t>
R base64_decode(const T* encoded_string, size_t in_len, bool remove_linebreaks = false, bool abort_on_invalid = false) {
	using valtype = typename R::value_type;

	if(remove_linebreaks) {

		if(!in_len)
			return R();

		std::basic_string<T> copy;
		copy.assign(encoded_string, in_len);

		copy.erase(std::remove_if(copy.begin(),
								  copy.end(),
								  [](T x)->bool {return x == static_cast<T>('\n'); }),
				   copy.end());

		return base64_decode<R, T>(copy.data(), copy.size(), false, abort_on_invalid);

	}

	size_t pos = 0;

	//
	// The approximate length (bytes) of the decoded string might be one ore
	// two bytes smaller, depending on the amount of trailing equal signs
	// in the encoded string. This approximation is needed to reserve
	// enough space in the string to be returned.
	//
	size_t approx_length_of_decoded_string = in_len / 4 * 3;
	R ret;
	ret.reserve(approx_length_of_decoded_string);

	while(pos < in_len) {
		auto pos_of_char_0 = pos_of_char(encoded_string[pos + 0]);
		auto pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);
		if(pos_of_char_0 == -1 || pos_of_char_1 == -1) {
			if(abort_on_invalid)
				ret.clear();
			break;
		}
		ret.push_back(static_cast<valtype>((pos_of_char_0 << 2) + ((pos_of_char_1 & 0x30) >> 4)));

		if(static_cast<char>(encoded_string[pos + 2]) != '=' && static_cast<char>(encoded_string[pos + 2]) != '.') { // accept URL-safe base 64 strings, too, so check for '.' also.

			auto pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
			if(pos_of_char_2 == -1) {
				if(abort_on_invalid)
					ret.clear();
				break;
			}
			ret.push_back(static_cast<valtype>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

			if(static_cast<char>(encoded_string[pos + 3]) != '=' && static_cast<char>(encoded_string[pos + 3]) != '.') {
				auto pos_of_char_3 = pos_of_char(encoded_string[pos + 3]);
				if(pos_of_char_3 == -1) {
					if(abort_on_invalid)
						ret.clear();
					break;
				}
				ret.push_back(static_cast<valtype>(((pos_of_char_2 & 0x03) << 6) + pos_of_char_3));
			}
		}
		pos += 4;
	}

	return ret;
}

template<typename R = std::vector<uint8_t>, typename T = std::wstring, typename valtype = typename T::value_type>
R base64_decode(const T& encoded_string, bool remove_linebreaks = false, bool abort_on_invalid = false) {
	return base64_decode<R, valtype>(encoded_string.data(), encoded_string.size(), remove_linebreaks, abort_on_invalid);
}

template<typename R = std::vector<uint8_t>, typename T = wchar_t>
R base64_decode(const T* encoded_string, bool remove_linebreaks = false, bool abort_on_invalid = false) {
	B64::basic_string_view<T> tmp{ encoded_string };
	return base64_decode<R, T>(tmp.data(), tmp.size(), remove_linebreaks, abort_on_invalid);
}
#endif //BASE64_H