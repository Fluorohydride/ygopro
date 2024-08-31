#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <cstdint>
#include "../ocgcore/buffer.h"

class BufferIO {
public:
	inline static int ReadInt32(unsigned char*& p) {
		return buffer_read<int32_t>(p);
	}
	inline static short ReadInt16(unsigned char*& p) {
		return buffer_read<int16_t>(p);
	}
	inline static char ReadInt8(unsigned char*& p) {
		return buffer_read<char>(p);
	}
	inline static unsigned char ReadUInt8(unsigned char*& p) {
		return buffer_read<unsigned char>(p);
	}
	inline static void WriteInt32(unsigned char*& p, int val) {
		buffer_write<int32_t>(p, val);
	}
	inline static void WriteInt16(unsigned char*& p, short val) {
		buffer_write<int16_t>(p, val);
	}
	inline static void WriteInt8(unsigned char*& p, char val) {
		buffer_write<char>(p, val);
	}
	// return: string length
	template<typename T1, typename T2>
	inline static int CopyWStr(const T1* src, T2* pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = (T2)src[l];
			l++;
		}
		pstr[l] = 0;
		return l;
	}
	template<typename T1, typename T2>
	inline static int CopyWStrRef(const T1* src, T2*& pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = (T2)src[l];
			l++;
		}
		pstr += l;
		*pstr = 0;
		return l;
	}
	template<typename T>
	static bool CheckStringSize(const T* str, int len) {
		for (int i = 0; i < len; ++i) {
			if (str[i] == 0)
				return false;
		}
		return true;
	}
	static bool IsHighSurrogate(unsigned int c) {
		return (c >= 0xd800U && c <= 0xdbffU);
	}
	static bool IsLowSurrogate(unsigned int c) {
		return (c >= 0xdc00U && c <= 0xdfffU);
	}
	static bool IsUnicodeChar(unsigned int c) {
		if(IsHighSurrogate(c))
			return false;
		if (IsLowSurrogate(c))
			return false;
		if (c > 0x10ffffU)
			return false;
		return true;
	}
	// UTF-16/UTF-32 to UTF-8
	// return: string length
	static int EncodeUTF8String(const wchar_t* wsrc, char* str, int size) {
		char* pstr = str;
		while (*wsrc != 0) {
			unsigned cur = *wsrc;
			int codepoint_size = 0;
			if (cur < 0x80U)
				codepoint_size = 1;
			else if (cur < 0x800U)
				codepoint_size = 2;
			else if (cur < 0x10000U && (cur < 0xd800U || cur > 0xdfffU))
				codepoint_size = 3;
			else
				codepoint_size = 4;
			if (pstr - str + codepoint_size > size - 1)
				break;
			switch (codepoint_size) {
			case 1:
				*pstr = (char)cur;
				break;
			case 2:
				pstr[0] = ((cur >> 6) & 0x1f) | 0xc0;
				pstr[1] = (cur & 0x3f) | 0x80;
				break;
			case 3:
				pstr[0] = ((cur >> 12) & 0xf) | 0xe0;
				pstr[1] = ((cur >> 6) & 0x3f) | 0x80;
				pstr[2] = (cur & 0x3f) | 0x80;
				break;
			case 4:
				if (sizeof(wchar_t) == 2) {
					cur = 0;
					cur |= (*wsrc & 0x3ffU) << 10;
					++wsrc;
					cur |= *wsrc & 0x3ffU;
					cur += 0x10000;
				}
				pstr[0] = ((cur >> 18) & 0x7) | 0xf0;
				pstr[1] = ((cur >> 12) & 0x3f) | 0x80;
				pstr[2] = ((cur >> 6) & 0x3f) | 0x80;
				pstr[3] = (cur & 0x3f) | 0x80;
				break;
			default:
				break;
			}
			pstr += codepoint_size;
			wsrc++;
		}
		*pstr = 0;
		return pstr - str;
	}
	// UTF-8 to UTF-16/UTF-32
	// return: string length
	static int DecodeUTF8String(const char* src, wchar_t* wstr, int size) {
		const char* p = src;
		wchar_t* wp = wstr;
		while(*p != 0) {
			const unsigned cur = *p & 0xffU;
			int codepoint_size = 0;
			if ((cur & 0xf8) == 0xf0) {
				if (sizeof(wchar_t) == 2)
					codepoint_size = 2;
				else
					codepoint_size = 1;
			}
			else
				codepoint_size = 1;
			if (wp - wstr + codepoint_size > size - 1)
				break;
			if((cur & 0x80) == 0) {
				*wp = *p;
				p++;
			} else if((cur & 0xe0) == 0xc0) {
				*wp = ((p[0] & 0x1fU) << 6) | (p[1] & 0x3fU);
				p += 2;
			} else if((cur & 0xf0) == 0xe0) {
				*wp = ((p[0] & 0xfU) << 12) | ((p[1] & 0x3fU) << 6) | (p[2] & 0x3fU);
				p += 3;
			} else if((cur & 0xf8) == 0xf0) {
				if (sizeof(wchar_t) == 2) {
					unsigned unicode = ((p[0] & 0x7U) << 18) | ((p[1] & 0x3fU) << 12) | ((p[2] & 0x3fU) << 6) | (p[3] & 0x3fU);
					unicode -= 0x10000;
					*wp++ = (unicode >> 10) | 0xd800;
					*wp = (unicode & 0x3ff) | 0xdc00;
				} else {
					*wp = ((p[0] & 0x7U) << 18) | ((p[1] & 0x3fU) << 12) | ((p[2] & 0x3fU) << 6) | (p[3] & 0x3fU);
				}
				p += 4;
			} else
				p++;
			wp++;
		}
		*wp = 0;
		return wp - wstr;
	}
	template<size_t N>
	static int EncodeUTF8(const wchar_t* src, char(&dst)[N]) {
		return EncodeUTF8String(src, dst, N);
	}
	template<size_t N>
	static int DecodeUTF8(const char* src, wchar_t(&dst)[N]) {
		return DecodeUTF8String(src, dst, N);
	}
	template<size_t N, typename T>
	static void NullTerminate(T(&str)[N]) {
		str[N - 1] = 0;
	}
	static int GetVal(const wchar_t* pstr) {
		unsigned int ret = 0;
		while(*pstr >= L'0' && *pstr <= L'9') {
			ret = ret * 10 + (*pstr - L'0');
			pstr++;
		}
		if (*pstr == 0)
			return (int)ret;
		return 0;
	}
};

#endif //BUFFERIO_H
