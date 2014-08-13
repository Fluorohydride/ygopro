#ifndef _BUFFERUTIL_H_
#define _BUFFERUTIL_H_

class BufferUtil {
public:
    BufferUtil(unsigned char* base) {
        ptr = base;
    }
    template<typename T>
    inline T Read() {
        T ret = *(T*)ptr;
        ptr += sizeof(T);
        return ret;
    }
    template<typename T>
    inline void Write(T val) {
        (*(T*)ptr) = val;
        ptr += sizeof(T);
    }
    inline void Skip(unsigned int offset) {
        ptr += offset;
    }
    static const char* ConvertInt(int val, char* buf) {
        sprintf(buf, "%d", val);
        return buf;
    }
    static const char* ConvertUInt(unsigned int val, char* buf) {
        sprintf(buf, "%u", val);
        return buf;
    }
    static const char* ConvertFloat(float val, char* buf) {
        sprintf(buf, "%f", val);
        return buf;
    }
    static const char* ConvertDouble(double val, char* buf) {
        sprintf(buf, "%lf", val);
        return buf;
    }
    static int EncodeUTF8(const wchar_t * wsrc, char * str) {
        char* pstr = str;
        while(*wsrc != 0) {
            if(*wsrc < 0x80) {
                *str = (char)*wsrc;
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
        return (int)(str - pstr);
    }
    static int DecodeUTF8(const char * src, wchar_t * wstr) {
        char* p = (char*)src;
        wchar_t* wp = wstr;
        while(*p != 0) {
            if((*p & 0x80) == 0) {
                *wp = *p;
                p++;
            } else if((*p & 0xe0) == 0xc0) {
                *wp = (((int)p[0] & 0x1f) << 6) | ((int)p[1] & 0x3f);
                p += 2;
            } else if((*p & 0xf0) == 0xe0) {
                *wp = (((int)p[0] & 0xf) << 12) | (((int)p[1] & 0x3f) << 6) | ((int)p[2] & 0x3f);
                p += 3;
            } else if((*p & 0xf8) == 0xf0) {
                *wp = (((int)p[0] & 0x7) << 18) | (((int)p[1] & 0x3f) << 12) | (((int)p[2] & 0x3f) << 6) | ((int)p[3] & 0x3f);
                p += 4;
            } else
                p++;
            wp++;
        }
        *wp = 0;
        return (int)(wp - wstr);
    }

private:
    unsigned char* ptr;

};

#endif //BUFFERIO_H
