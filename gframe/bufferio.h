#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <cstdint>
#include <cstring>
#include <cwchar>

class BufferIO {
public:
	template<typename T>
	static T Read(unsigned char*& p) {
		T ret{};
		std::memcpy(&ret, p, sizeof(T));
		p += sizeof(T);
		return ret;
	}
	template<typename T>
	static void Write(unsigned char*& p, const T& value) {
		std::memcpy(p, &value, sizeof(T));
		p += sizeof(T);
	}

	static void VectorWriteBlock(std::vector<unsigned char>& buffer, const void* src, size_t size) {
		const auto len = buffer.size();
		buffer.resize(len + size);
		std::memcpy(buffer.data() + len, src, size);
	}
	template<typename T>
	static void VectorWrite(std::vector<unsigned char>& buffer, const T& value) {
		VectorWriteBlock(buffer, &value, sizeof(T));
	}

	// for compatibility
	[[deprecated]]
	static int32_t ReadInt32(unsigned char*& p) {
		return Read<int32_t>(p);
	}
	[[deprecated]]
	static short ReadInt16(unsigned char*& p) {
		return Read<int16_t>(p);
	}
	[[deprecated]]
	static char ReadInt8(unsigned char*& p) {
		return Read<char>(p);
	}
	[[deprecated]]
	static unsigned char ReadUInt8(unsigned char*& p) {
		return Read<unsigned char>(p);
	}
	[[deprecated]]
	static void WriteInt32(unsigned char*& p, int32_t val) {
		Write<int32_t>(p, val);
	}
	[[deprecated]]
	static void WriteInt16(unsigned char*& p, short val) {
		Write<int16_t>(p, val);
	}
	[[deprecated]]
	static void WriteInt8(unsigned char*& p, char val) {
		Write<char>(p, val);
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
	static void CopyString(const char* src, char(&dst)[N], size_t len = N - 1) {
		if(len >= N)
			len = N - 1;
		std::strncpy(dst, src, len);
		dst[len] = 0;
	}
	template<size_t N>
	static void CopyWideString(const wchar_t* src, wchar_t(&dst)[N], size_t len = N - 1) {
		if(len >= N)
			len = N - 1;
		std::wcsncpy(dst, src, len);
		dst[len] = 0;
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
