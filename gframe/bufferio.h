#ifndef BUFFERIO_H
#define BUFFERIO_H

#include <string>
#include <vector>
#include <cstring>

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
	inline static void Read(char*& p, void* dest, size_t size) {
		memcpy(dest, p, size);
		p += size;
	}
	template<typename T>
	inline static T Read(char*& p) {
		T ret;
		memcpy((void*)&ret, p, sizeof(T));
		p += sizeof(T);
		return ret;
	}
	template<typename T>
	inline static void Write(char*& p, T value) {
		std::memcpy(p, &value, sizeof(T));
		p += sizeof(T);
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
				*str = static_cast<char>(*wsrc);
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
				if(sizeof(wchar_t) == 2) {
					unsigned unicode = 0;
					unicode |= (*wsrc++ & 0x3ff) << 10;
					unicode |= *wsrc & 0x3ff;
					unicode += 0x10000;
					str[0] = ((unicode >> 18) & 0x7) | 0xf0;
					str[1] = ((unicode >> 12) & 0x3f) | 0x80;
					str[2] = ((unicode >> 6) & 0x3f) | 0x80;
					str[3] = ((unicode) & 0x3f) | 0x80;
				} else {
					str[0] = ((*wsrc >> 18) & 0x7) | 0xf0;
					str[1] = ((*wsrc >> 12) & 0x3f) | 0x80;
					str[2] = ((*wsrc >> 6) & 0x3f) | 0x80;
					str[3] = ((*wsrc) & 0x3f) | 0x80;
				}
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
				if(sizeof(wchar_t) == 2) {
					unsigned unicode = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
					unicode -= 0x10000;
					*wp++ = (unicode >> 10) | 0xd800;
					*wp = (unicode & 0x3ff) | 0xdc00;
				} else {
					*wp = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
				}
				p += 4;
			} else
				p++;
			wp++;
		}
		*wp = 0;
		return wp - wstr;
	}
	static std::string EncodeUTF8s(const std::wstring& source) {
		thread_local std::vector<char> res;
		res.reserve(source.size() * 4 + 1);
		EncodeUTF8(source.c_str(), const_cast<char*>(res.data()));
		return res.data();
	}
	// UTF-8 to UTF-16/UTF-32
	static std::wstring DecodeUTF8s(const std::string& source) {
		thread_local std::vector<wchar_t> res;
		if(sizeof(wchar_t) == 2)
			res.reserve(source.size() * 2 + 1);
		else
			res.reserve(source.size() + 1);
		DecodeUTF8(source.c_str(), const_cast<wchar_t*>(res.data()));
		return res.data();
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
