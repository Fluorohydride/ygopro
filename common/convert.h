#ifndef _CONVERT_H_
#define _CONVERT_H_

template<typename T>
struct ConverteImpl {
    static T ConvertVal(const char* str) {
        if(std::is_integral<T>::value) {
            const char* s = str;
            int radix = (str[0] == '0' && (str[1] == 'X' || str[1] == 'x')) ? 16 : 10;
            return static_cast<T>(strtoull(s, nullptr, radix));
        } else
            return static_cast<T>(atof(str));
    }
    static std::string ConvertStr(const T& val) {
        char buf[256];
        if(std::is_integral<T>::value)
            sprintf(buf, "%ld", static_cast<long>(val));
        else
            sprintf(buf, "%lf", static_cast<double>(val));
        return std::move(std::string(buf));
    }
};

template<typename T, typename TOTYPE>
struct ToInner {
    static TOTYPE C(const T& str) {
        return static_cast<TOTYPE>(str);
    };
};

template<typename TOTYPE>
struct ToInner<char*, TOTYPE> {
    static TOTYPE C(const char* str) {
        return static_cast<TOTYPE>(ConverteImpl<TOTYPE>::ConvertVal(str));
    };
};

template<typename TOTYPE>
struct ToInner<const char*, TOTYPE> {
    static TOTYPE C(const char* str) {
        return static_cast<TOTYPE>(ConverteImpl<TOTYPE>::ConvertVal(str));
    };
};

template<typename TOTYPE>
struct ToInner<std::string, TOTYPE> {
    static TOTYPE C(const std::string& str) {
        return static_cast<TOTYPE>(ConverteImpl<TOTYPE>::ConvertVal(str.c_str()));
    };
};

template<typename T>
struct ToInner<T, std::string> {
    static std::string C(const T& val) {
        return std::move(ConverteImpl<T>::ConvertStr(val));
    }
};

template<>
struct ToInner<std::wstring, std::string> {
    static std::string C(const std::wstring& val) {
        char buffer[2048], *str = buffer;
        const wchar_t* wsrc = val.c_str();
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
        return std::move(std::string(buffer));
    }
};

template<>
struct ToInner<std::string, std::wstring> {
    static std::wstring C(const std::string& val) {
        wchar_t buffer[1024], *wstr = buffer;
        const char* p = val.c_str();
        while(*p != 0) {
            if((*p & 0x80) == 0) {
                *wstr = *p;
                p++;
            } else if((*p & 0xe0) == 0xc0) {
                *wstr = (((int)p[0] & 0x1f) << 6) | ((int)p[1] & 0x3f);
                p += 2;
            } else if((*p & 0xf0) == 0xe0) {
                *wstr = (((int)p[0] & 0xf) << 12) | (((int)p[1] & 0x3f) << 6) | ((int)p[2] & 0x3f);
                p += 3;
            } else if((*p & 0xf8) == 0xf0) {
                *wstr = (((int)p[0] & 0x7) << 18) | (((int)p[1] & 0x3f) << 12) | (((int)p[2] & 0x3f) << 6) | ((int)p[3] & 0x3f);
                p += 4;
            } else
                p++;
            wstr++;
        }
        *wstr = 0;
        return std::move(std::wstring(buffer));
    }
};

template<typename TOTYPE, typename T>
TOTYPE To(const T& val) {
    return ToInner<T, TOTYPE>::C(val);
}

template<typename TOTYPE, typename T, typename... REST>
TOTYPE To(const char* format, const T& val, const REST&... r) {
    char buf[256];
    sprintf(buf, format, val, r...);
    return std::move(std::string(buf));
}

#endif
