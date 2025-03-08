#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <cstdint>
#include <cwchar>
#include "../ocgcore/buffer.h"

class BufferIO {
public:
	static int ReadInt32(unsigned char*& p) {
		return buffer_read<int32_t>(p);
	}
	static short ReadInt16(unsigned char*& p) {
		return buffer_read<int16_t>(p);
	}
	static char ReadInt8(unsigned char*& p) {
		return buffer_read<char>(p);
	}
	static unsigned char ReadUInt8(unsigned char*& p) {
		return buffer_read<unsigned char>(p);
	}
	static void WriteInt32(unsigned char*& p, int val) {
		buffer_write<int32_t>(p, val);
	}
	static void WriteInt16(unsigned char*& p, short val) {
		buffer_write<int16_t>(p, val);
	}
	static void WriteInt8(unsigned char*& p, char val) {
		buffer_write<char>(p, val);
	}
	/**
	* @brief Copy a C-style string to another C-style string.
	* @param src The source wide string
	* @param pstr The destination char string
	* @param bufsize The size of the destination buffer
	* @return The length of the copied string
	*/
	template<typename T1, typename T2>
	static int CopyWStr(const T1* src, T2* pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = (T2)src[l];
			l++;
		}
		pstr[l] = 0;
		return l;
	}
	template<typename T1, typename T2>
	static int CopyWStrRef(const T1* src, T2*& pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = (T2)src[l];
			l++;
		}
		pstr += l;
		*pstr = 0;
		return l;
	}
	template<typename T1, typename T2, size_t N>
	static int CopyCharArray(const T1* src, T2(&dst)[N]) {
		return CopyWStr(src, dst, N);
	}
	template<size_t N>
	static void CopyString(const char* src, char(&dst)[N]) {
		dst[0] = 0;
		std::strncat(dst, src, N - 1);
	}
	template<size_t N>
	static void CopyWideString(const wchar_t* src, wchar_t(&dst)[N]) {
		dst[0] = 0;
		std::wcsncat(dst, src, N - 1);
	}
	template<typename T>
	static bool CheckUTF8Byte(const T* str, int len) {
		for (int i = 1; i < len; ++i) {
			if ((str[i] & 0xc0U) != 0x80U)
				return false;
		}
		return true;
	}
	static unsigned int ConvertUTF8(const char*& p) {
		unsigned int cur = 0;
		if ((p[0] & 0x80U) == 0) {
			cur = p[0] & 0xffU;
			p++;
		}
		else if ((p[0] & 0xe0U) == 0xc0U) {
			if (!CheckUTF8Byte(p, 2)) {
				p++;
				return UINT32_MAX;
			}
			cur = ((p[0] & 0x1fU) << 6) | (p[1] & 0x3fU);
			p += 2;
			if(cur < 0x80U)
				return UINT32_MAX;
		}
		else if ((p[0] & 0xf0U) == 0xe0U) {
			if (!CheckUTF8Byte(p, 3)) {
				p++;
				return UINT32_MAX;
			}
			cur = ((p[0] & 0xfU) << 12) | ((p[1] & 0x3fU) << 6) | (p[2] & 0x3fU);
			p += 3;
			if (cur < 0x800U)
				return UINT32_MAX;
		}
		else if ((p[0] & 0xf8U) == 0xf0U) {
			if (!CheckUTF8Byte(p, 4)) {
				p++;
				return UINT32_MAX;
			}
			cur = ((p[0] & 0x7U) << 18) | ((p[1] & 0x3fU) << 12) | ((p[2] & 0x3fU) << 6) | (p[3] & 0x3fU);
			p += 4;
			if (cur < 0x10000U)
				return UINT32_MAX;
		}
		else {
			p++;
			return UINT32_MAX;
		}
		return cur;
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
		const auto *pw = wsrc;
		auto *pstr = str;
		while (*pw != 0) {
			unsigned cur = 0;
			int codepoint_size = 0;
			if (sizeof(wchar_t) == 2) {
				if (IsHighSurrogate(pw[0])) {
					if (pw[1] == 0)
						break;
					if (IsLowSurrogate(pw[1])) {
						cur = ((pw[0] & 0x3ffU) << 10) | (pw[1] & 0x3ffU);
						cur += 0x10000;
						pw += 2;
					}
					else {
						pw++;
						continue;
					}
				}
				else if (IsLowSurrogate(pw[0])) {
					pw++;
					continue;
				}
				else {
					cur = *pw;
					pw++;
				}
			}
			else {
				cur = *pw;
				pw++;
			}
			if (!IsUnicodeChar(cur))
				continue;
			if (cur < 0x80U)
				codepoint_size = 1;
			else if (cur < 0x800U)
				codepoint_size = 2;
			else if (cur < 0x10000U)
				codepoint_size = 3;
			else
				codepoint_size = 4;
			if ((int)(pstr - str) + codepoint_size > size - 1)
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
				pstr[0] = ((cur >> 18) & 0x7) | 0xf0;
				pstr[1] = ((cur >> 12) & 0x3f) | 0x80;
				pstr[2] = ((cur >> 6) & 0x3f) | 0x80;
				pstr[3] = (cur & 0x3f) | 0x80;
				break;
			default:
				break;
			}
			pstr += codepoint_size;
		}
		*pstr = 0;
		return (int)(pstr - str);
	}
	// UTF-8 to UTF-16/UTF-32
	// return: string length
	static int DecodeUTF8String(const char* src, wchar_t* wstr, int size) {
		const char* p = src;
		wchar_t* wp = wstr;
		while(*p != 0) {
			unsigned int cur = ConvertUTF8(p);
			int codepoint_size = 0;
			if (!IsUnicodeChar(cur))
				continue;
			if (cur >= 0x10000) {
				if (sizeof(wchar_t) == 2)
					codepoint_size = 2;
				else
					codepoint_size = 1;
			}
			else
				codepoint_size = 1;
			if ((int)(wp - wstr) + codepoint_size > size - 1)
				break;
			if (codepoint_size == 1) {
				wp[0] = cur;
				wp++;
			}
			else {
				cur -= 0x10000U;
				wp[0] = (cur >> 10) | 0xd800;
				wp[1] = (cur & 0x3ff) | 0xdc00;
				wp += 2;
			}
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
		if (*pstr >= L'0' && *pstr <= L'9') {
			int ret{};
			wchar_t* str_end{};
			ret = std::wcstol(pstr, &str_end, 10);
			if (*str_end == 0)
				return ret;
			else
				return 0;
		}
		else
			return 0;
	
	}
};

#endif //BUFFERIO_H
