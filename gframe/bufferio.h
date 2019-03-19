#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <string>
#include <vector>
#include <cstring>

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif

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
		const auto vec_size = vec.size();
		const auto val_size = sizeof(T);
		vec.resize(vec_size + val_size);
		std::memcpy(&vec[vec_size], &val, val_size);
	}
	template<typename T>
	inline static T Read(char*& p) {
		T ret;
		memcpy((void*)&ret, p, sizeof(T));
		p += sizeof(T);
		return ret;
	}
	inline static int64_t ReadInt64(char*& p) {
		return Read<int64_t>(p);
	}
	inline static int32_t ReadInt32(char*& p) {
		return Read<int32_t>(p);
	}
	inline static int16_t ReadInt16(char*& p) {
		return Read<int16_t>(p);
	}
	inline static int8_t ReadInt8(char*& p) {
		return Read<int8_t>(p);
	}
	inline static uint8_t ReadUInt8(char*& p) {
		return Read<uint8_t>(p);
	}
	template<typename T>
	inline static void Write(char*& p, T value) {
		std::memcpy(p, &value, sizeof(T));
		p += sizeof(T);
	}
	inline static void WriteInt64(char*& p, long long val) {
		Write<int64_t>(p, val);
	}
	inline static void WriteInt32(char*& p, int val) {
		Write<int32_t>(p, val);
	}
	inline static void WriteInt16(char*& p, short val) {
		Write<int16_t>(p, val);
	}
	inline static void WriteInt8(char*& p, char val) {
		Write<int8_t>(p, val);
	}
	template<typename T1, typename T2>
	inline static int CopyWStr(T1* src, T2* pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = src[l];
			l++;
		}
		pstr[l] = 0;
		return l;
	}
	template<typename T1, typename T2>
	inline static int CopyWStrRef(T1* src, T2*& pstr, int bufsize) {
		int l = 0;
		while(src[l] && l < bufsize - 1) {
			pstr[l] = src[l];
			l++;
		}
		pstr += l;
		*pstr = 0;
		return l;
	}
	// UTF-16/UTF-32 to UTF-8
	static int EncodeUTF8(const wchar_t * wsrc, char * str) {
		char* pstr = str;
		while(*wsrc != 0) {
			if(*wsrc < 0x80) {
				*str = *wsrc;
				++str;
			} else if(*wsrc < 0x800) {
				str[0] = ((*wsrc >> 6) & 0x1f) | 0xc0;
				str[1] = ((*wsrc) & 0x3f) | 0x80;
				str += 2;
			} else if(*wsrc < 0x10000 && (*wsrc < 0xd800 || *wsrc > 0xdfff)) {
				str[0] = ((*wsrc >> 12) & 0xf) | 0xe0;
				str[1] = ((*wsrc >> 6) & 0x3f) | 0x80;
				str[2] = ((*wsrc) & 0x3f) | 0x80;
				str += 3;
			} else {
#ifdef _WIN32
				unsigned unicode = 0;
				unicode |= (*wsrc++ & 0x3ff) << 10;
				unicode |= *wsrc & 0x3ff;
				unicode += 0x10000;
				str[0] = ((unicode >> 18) & 0x7) | 0xf0;
				str[1] = ((unicode >> 12) & 0x3f) | 0x80;
				str[2] = ((unicode >> 6) & 0x3f) | 0x80;
				str[3] = ((unicode) & 0x3f) | 0x80;
#else
				str[0] = ((*wsrc >> 18) & 0x7) | 0xf0;
				str[1] = ((*wsrc >> 12) & 0x3f) | 0x80;
				str[2] = ((*wsrc >> 6) & 0x3f) | 0x80;
				str[3] = ((*wsrc) & 0x3f) | 0x80;
#endif // _WIN32
				str += 4;
			}
			wsrc++;
		}
		*str = 0;
		return str - pstr;
	}
	// UTF-8 to UTF-16/UTF-32
	static int DecodeUTF8(const char * src, wchar_t * wstr) {
		const char* p = src;
		wchar_t* wp = wstr;
		while(*p != 0) {
			if((*p & 0x80) == 0) {
				*wp = *p;
				p++;
			} else if((*p & 0xe0) == 0xc0) {
				*wp = (((unsigned)p[0] & 0x1f) << 6) | ((unsigned)p[1] & 0x3f);
				p += 2;
			} else if((*p & 0xf0) == 0xe0) {
				*wp = (((unsigned)p[0] & 0xf) << 12) | (((unsigned)p[1] & 0x3f) << 6) | ((unsigned)p[2] & 0x3f);
				p += 3;
			} else if((*p & 0xf8) == 0xf0) {
#ifdef _WIN32
				unsigned unicode = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
				unicode -= 0x10000;
				*wp++ = (unicode >> 10) | 0xd800;
				*wp = (unicode & 0x3ff) | 0xdc00;
#else
				*wp = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
#endif // _WIN32
				p += 4;
			} else
				p++;
			wp++;
		}
		*wp = 0;
		return wp - wstr;
	}
	static std::string EncodeUTF8s(const std::wstring& source) {
		std::string res;
		for(size_t i = 0; i < source.size(); i++) {
			auto c = source[i];
			if(c < 0x80) {
				res += ((char)c);
			} else if(c < 0x800) {
				res += ((char)(((c >> 6) & 0x1f) | 0xc0));
				res += ((char)((c & 0x3f) | 0x80));
			} else if(c < 0x10000 && (c < 0xd800 || c > 0xdfff)) {
				res += ((char)(((c >> 12) & 0xf) | 0xe0));
				res += ((char)(((c >> 6) & 0x3f) | 0x80));
				res += ((char)(((c) & 0x3f) | 0x80));
			} else {
#ifdef _WIN32
				unsigned unicode = 0;
				unicode |= (c & 0x3ff) << 10;
				c = source[++i];
				unicode |= c & 0x3ff;
				unicode += 0x10000;
				res += ((char)(((unicode >> 18) & 0x7) | 0xf0));
				res += ((char)(((unicode >> 12) & 0x3f) | 0x80));
				res += ((char)(((unicode >> 6) & 0x3f) | 0x80));
				res += ((char)(((unicode) & 0x3f) | 0x80));
#else
				res += ((char)(((c >> 18) & 0x7) | 0xf0));
				res += ((char)(((c >> 12) & 0x3f) | 0x80));
				res += ((char)(((c >> 6) & 0x3f) | 0x80));
				res += ((char)(((c) & 0x3f) | 0x80));
#endif
			}
		}
		return res;
	}
	// UTF-8 to UTF-16/UTF-32
	static std::wstring DecodeUTF8s(const std::string& source) {
		std::wstring res = L"";
		for(size_t i = 0; i < source.size();) {
			auto c = source[i];
			if((c & 0x80) == 0) {
				res += ((wchar_t)c);
				i++;
			} else if((c & 0xe0) == 0xc0) {
				res += ((wchar_t)((((unsigned)c & 0x1f) << 6) | ((unsigned)source[i + 1] & 0x3f)));
				i += 2;
			} else if((c & 0xf0) == 0xe0) {
				res += ((wchar_t)((((unsigned)c & 0xf) << 12) | (((unsigned)source[i + 1] & 0x3f) << 6) | ((unsigned)source[i + 2] & 0x3f)));
				i += 3;
			} else if((c & 0xf8) == 0xf0) {
#ifdef _WIN32
				unsigned unicode = (((unsigned)c & 0x7) << 18) | (((unsigned)source[i + 1] & 0x3f) << 12) | (((unsigned)source[i + 2] & 0x3f) << 6) | ((unsigned)source[i + 3] & 0x3f);
				unicode -= 0x10000;
				res += ((wchar_t)((unicode >> 10) | 0xd800));
				res += ((wchar_t)((unicode & 0x3ff) | 0xdc00));
#else
				res += ((wchar_t)((((unsigned)c & 0x7) << 18) | (((unsigned)source[i + 1] & 0x3f) << 12) | (((unsigned)source[i + 2] & 0x3f) << 6) | ((unsigned)source[i + 3] & 0x3f)));
#endif // _WIN32
				i += 4;
			} else
				i++;
		}
		return res;
	}
	static int GetVal(const wchar_t* pstr) {
		int ret = 0;
		while(*pstr >= L'0' && *pstr <= L'9') {
			ret = ret * 10 + (*pstr - L'0');
			pstr++;
		}
		if(*pstr == 0)
			return ret;
		return 0;
	}
};

#endif //BUFFERIO_H
