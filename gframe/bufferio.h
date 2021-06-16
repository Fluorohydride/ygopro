#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <string>
#include <vector>
#include <cstring>
#include <wchar.h>
#include <cstdint>
#include "text_types.h"

class BufferIO {
public:
	static void insert_data(std::vector<uint8_t>& vec, void* val, size_t len) {
		const auto vec_size = vec.size();
		const auto val_size = len;
		vec.resize(vec_size + val_size);
		std::memcpy(&vec[vec_size], val, val_size);
	}
	template<typename T>
	static void insert_value(std::vector<uint8_t>& vec, T val) {
		insert_data(vec, &val, sizeof(T));
	}
	inline static void Read(char*& p, void* dest, size_t size) {
		std::memcpy(dest, p, size);
		p += size;
	}
	template<typename T>
	inline static T Read(char*& p) {
		T ret;
		Read(p, &ret, sizeof(T));
		return ret;
	}
	template<typename T>
	inline static void Write(char*& p, T value) {
		std::memcpy(p, &value, sizeof(T));
		p += sizeof(T);
	}
	template<typename T>
	inline static int CopyStr(const T* src, T* pstr, int bufsize) {
		auto p = (const uint8_t*)(src);
		for(const auto* tg = p + (bufsize * sizeof(T)); p <= tg; p += sizeof(T), pstr++) {
			std::memcpy(pstr, p, sizeof(T));
			if(!*pstr)
				break;
		}
		return (const T*)p - src;
	}
private:
	template<bool check = false>
	static int EncodeUTF8internal(const wchar_t* wsrc, char* out, size_t size = 0) {
		(void)size;
		char* pstr = out;
		auto GetNextSize = [](wchar_t cur) -> size_t {
			if(cur < 0x80)
				return 1;
			if(cur < 0x800)
				return 2;
			if(cur < 0x10000 && (cur < 0xd800 || cur > 0xdfff))
				return 3;
			return 4;
		};
		while(*wsrc != 0) {
			const auto codepoint_size = GetNextSize(*wsrc);
			if(check && size != 0 && ((out - pstr) + codepoint_size) >= (size - 1))
				break;
			switch(codepoint_size) {
			case 1:
				*out = static_cast<char>(*wsrc);
				break;
			case 2:
				out[0] = ((*wsrc >> 6) & 0x1f) | 0xc0;
				out[1] = ((*wsrc) & 0x3f) | 0x80;
				break;
			case 3:
				out[0] = ((*wsrc >> 12) & 0xf) | 0xe0;
				out[1] = ((*wsrc >> 6) & 0x3f) | 0x80;
				out[2] = ((*wsrc) & 0x3f) | 0x80;
				break;
			case 4:
				uint32_t unicode = 0;
				if(sizeof(wchar_t) == 2) {
					unicode |= (*wsrc++ & 0x3ff) << 10;
					unicode |= *wsrc & 0x3ff;
					unicode += 0x10000;
				} else
					unicode = *wsrc;
				out[0] = ((unicode >> 18) & 0x7) | 0xf0;
				out[1] = ((unicode >> 12) & 0x3f) | 0x80;
				out[2] = ((unicode >> 6) & 0x3f) | 0x80;
				out[3] = (unicode & 0x3f) | 0x80;
				break;
			}
			out += codepoint_size;
			wsrc++;
		}
		*out = 0;
		return out - pstr;
	}
	template<bool check = false>
	static int DecodeUTF8internal(const char* src, wchar_t* out, size_t size = 0) {
		(void)size;
		wchar_t* pstr = out;
		while(*src != 0) {
			if(check && size != 0) {
				const size_t len = out - pstr;
				if(len >= (size - 1))
					break;
				if(sizeof(wchar_t) == 2 && (*src & 0xf8) == 0xf0 && len >= (size - 2))
					break;
			}
			if((*src & 0x80) == 0) {
				*out = *src;
				src++;
			} else if((*src & 0xe0) == 0xc0) {
				*out = ((src[0] & 0x1f) << 6) | (src[1] & 0x3f);
				src += 2;
			} else if((*src & 0xf0) == 0xe0) {
				*out = ((src[0] & 0xf) << 12) | ((src[1] & 0x3f) << 6) | (src[2] & 0x3f);
				src += 3;
			} else if((*src & 0xf8) == 0xf0) {
				if(sizeof(wchar_t) == 2) {
					uint32_t unicode = ((src[0] & 0x7) << 18) | ((src[1] & 0x3f) << 12) | ((src[2] & 0x3f) << 6) | (src[3] & 0x3f);
					unicode -= 0x10000;
					*out++ = static_cast<wchar_t>((unicode >> 10) | 0xd800);
					*out = static_cast<wchar_t>((unicode & 0x3ff) | 0xdc00);
				} else {
					*out = static_cast<wchar_t>(((src[0] & 0x7) << 18) | ((src[1] & 0x3f) << 12) | ((src[2] & 0x3f) << 6) | (src[3] & 0x3f));
				}
				src += 4;
			} else
				src++;
			out++;
		}
		*out = 0;
		return out - pstr;
	}
	template<bool check = false>
	static int EncodeUTF16internal(const wchar_t* source, uint16_t* out, size_t size = 0) {
		auto* pstr = out;
		while(*source) {
			const auto cur = *source++;
			if(check && size != 0) {
				const size_t len = out - pstr;
				if(len >= (size - 1))
					break;
				if(cur >= 0x10000 && len >= (size - 2))
					break;
			}
			if(cur < 0x10000) {
				*out++ = cur;
			} else {
				unsigned int unicode = cur - 0x10000;
				*out++ = static_cast<wchar_t>((unicode >> 10) | 0xd800);
				*out = static_cast<wchar_t>((unicode & 0x3ff) | 0xdc00);
			}
		}
		*out = 0;
		return out - pstr;
	}
	template<bool check = false>
	static int DecodeUTF16internal(const uint16_t* source, wchar_t* out, size_t size = 0) {
		wchar_t* pstr = out;
		while(*source) {
			if(check && size != 0) {
				const size_t len = out - pstr;
				if(len >= (size - 1))
					break;
			}
			auto cur = *source++;
			if((cur - 0xd800u) >= 0x800u) {
				*out++ = static_cast<wchar_t>(cur);
			} else if((cur & 0xfffffc00) == 0xd800u && (*source & 0xfffffc00u) == 0xdc00u) {
				*out++ = (cur << 10) + (*source++) - 0x35fdc00u;
			}
		}
		*out = 0;
		return out - pstr;
	}
public:
	// UTF-16/UTF-32 to UTF-8
	static inline int EncodeUTF8(const wchar_t* wsrc, char* out, size_t size) {
		return EncodeUTF8internal<true>(wsrc, out, size);
	}
	static inline int EncodeUTF8(const wchar_t* wsrc, char* out) {
		return EncodeUTF8internal<false>(wsrc, out);
	}
	static std::string EncodeUTF8(epro::wstringview source) {
		std::string res(source.size() * 4 + 1, L'\0');
		res.resize(EncodeUTF8(source.data(), &res[0]));
		return res;
	}
	// UTF-8 to UTF-16/UTF-32
	static inline int DecodeUTF8(const char* src, wchar_t* out, size_t size) {
		return DecodeUTF8internal<true>(src, out, size);
	}
	static inline int DecodeUTF8(const char* src, wchar_t* out) {
		return DecodeUTF8internal<false>(src, out);
	}
	static std::wstring DecodeUTF8(epro::stringview source) {
		std::wstring res(source.size() + 1, '\0');
		res.resize(DecodeUTF8(source.data(), &res[0]));
		return res;
	}
	// UTF-16 to UTF-16/UTF-32
	static inline int DecodeUTF16(const uint16_t* source, wchar_t* out, size_t size) {
		if(sizeof(wchar_t) == sizeof(uint16_t)) {
			wcsncpy(out, (const wchar_t*)source, size - 1);
			out[size - 1] = L'\0';
			return wcslen(out) + 1;
		} else {
			return DecodeUTF16internal<true>(source, out, size) + 1;
		}
	}
	static inline int DecodeUTF16(const uint16_t* source, wchar_t* out) {
		if(sizeof(wchar_t) == sizeof(uint16_t)) {
			wcscpy(out, (const wchar_t*)source);
			return wcslen(out) + 1;
		} else {
			return DecodeUTF16internal<false>(source, out) + 1;
		}
	}
	// UTF-16/UTF-32 to UTF-16
	static inline int EncodeUTF16(const wchar_t* source, uint16_t* out, size_t size) {
		if(sizeof(wchar_t) == sizeof(uint16_t)) {
			wcsncpy((wchar_t*)out, source, size - 1);
			out[size - 1] = L'\0';
			return wcslen((wchar_t*)out) + 1;
		} else {
			return EncodeUTF16internal<true>(source, out, size) + 1;
		}
	}
	static inline int EncodeUTF16(const wchar_t* source, uint16_t* out) {
		if(sizeof(wchar_t) == sizeof(uint16_t)) {
			wcscpy((wchar_t*)out, source);
			return wcslen((wchar_t*)out) + 1;
		} else {
			return EncodeUTF16internal<false>(source, out) + 1;
		}
	}
	static uint32_t GetVal(const wchar_t* pstr) {
		uint32_t ret = 0;
		while(*pstr >= L'0' && *pstr <= L'9') {
			ret = ret * 10 + (*pstr - L'0');
			pstr++;
		}
		if(*pstr == 0)
			return ret;
		return 0;
	}

	template<typename T>
	static T getStruct(void* data, size_t len) {
		T pkt{};
		memcpy(&pkt, data, std::min<size_t>(sizeof(T), len));
		return pkt;
	}
};

#endif //BUFFERIO_H
