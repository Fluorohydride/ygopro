#ifndef BUFFERIO_H
#define BUFFERIO_H

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif

class BufferIO {
public:
	inline static int ReadInt32(char*& p) {
		int ret = *(int*)p;
		p += 4;
		return ret;
	}
	inline static short ReadInt16(char*& p) {
		short ret = *(short*)p;
		p += 2;
		return ret;
	}
	inline static char ReadInt8(char*& p) {
		char ret = *(char*)p;
		p++;
		return ret;
	}
	inline static unsigned char ReadUInt8(char*& p) {
		unsigned char ret = *(unsigned char*)p;
		p++;
		return ret;
	}
	inline static void WriteInt32(char*& p, int val) {
		(*(int*)p) = val;
		p += 4;
	}
	inline static void WriteInt16(char*& p, short val) {
		(*(short*)p) = val;
		p += 2;
	}
	inline static void WriteInt8(char*& p, char val) {
		*p = val;
		p++;
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
	// UCS-2 to UTF-8
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
			} else {
				str[0] = ((*wsrc >> 12) & 0xf) | 0xe0;
				str[1] = ((*wsrc >> 6) & 0x3f) | 0x80;
				str[2] = ((*wsrc) & 0x3f) | 0x80;
				str += 3;
			}
			wsrc++;
		}
		*str = 0;
		return str - pstr;
	}
	// UTF-8 to UCS-2
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
				*wp = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
				p += 4;
			} else
				p++;
			wp++;
		}
		*wp = 0;
		return wp - wstr;
	}
	static int GetVal(const wchar_t* pstr) {
		int ret = 0;
		while(*pstr >= L'0' && *pstr <= L'9') {
			ret = ret * 10 + (*pstr - L'0');
			pstr++;
		}
		return ret;
	}
};

#endif //BUFFERIO_H
