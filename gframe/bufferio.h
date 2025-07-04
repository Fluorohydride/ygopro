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
	* @param bufsize The length of the destination buffer
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
		std::strncpy(dst, src, N - 1);
		dst[N - 1] = 0;
	}
	template<size_t N>
	static void CopyWideString(const wchar_t* src, wchar_t(&dst)[N]) {
		std::wcsncpy(dst, src, N - 1);
		dst[N - 1] = 0;
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
	static int EncodeUTF8String(const wchar_t* wsrc, char* str, size_t len) {
		if (len == 0) {
			str[0] = 0;
			return 0;
		}
		std::mbstate_t state{};
		size_t result_len = std::wcsrtombs(str, &wsrc, len - 1, &state);
		if (result_len == static_cast<size_t>(-1))
			result_len = 0;
		str[result_len] = 0;
		return static_cast<int>(result_len);
	}
	// UTF-8 to UTF-16/UTF-32
	// return: string length
	static int DecodeUTF8String(const char* src, wchar_t* wstr, size_t len) {
		if (len == 0) {
			wstr[0] = 0;
			return 0;
		}
		std::mbstate_t state{};
		size_t result_len = std::mbsrtowcs(wstr, &src, len - 1, &state);
		if (result_len == static_cast<size_t>(-1))
			result_len = 0;
		wstr[result_len] = 0;
		return static_cast<int>(result_len);
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
