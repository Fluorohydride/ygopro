#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "common.h"

template<typename T, typename STYPE, bool INTTYPE = true>
struct SConverter {
    static T Convert(const STYPE& str) {
        try{
            return static_cast<T>(std::stoll(str, nullptr, 0));
        } catch(...) {
            return 0;
        }
    }
};

template<typename T, typename STYPE>
struct SConverter<T, STYPE, false> {
    static T Convert(const STYPE& str) {
        try{
            return static_cast<T>(std::stod(str));
        } catch(...) {
            return 0.0;
        }
    }
};

template<typename T, typename TOTYPE>
struct ToInner {
    static TOTYPE C(const T& str) {
        return static_cast<TOTYPE>(str);
    };
};

template<typename TOTYPE>
struct ToInner<std::string, TOTYPE> {
    static TOTYPE C(const std::string& str) {
        return static_cast<TOTYPE>(SConverter<TOTYPE, std::string, std::is_integral<TOTYPE>::value>::Convert(str));
    };
};

template<typename TOTYPE>
struct ToInner<std::wstring, TOTYPE> {
    static TOTYPE C(const std::wstring& str) {
        return static_cast<TOTYPE>(SConverter<TOTYPE, std::wstring, std::is_integral<TOTYPE>::value>::Convert(str));
    };
};

template<typename T>
struct ToInner<T, std::string> {
    static std::string C(const T& val) {
        return std::move(std::to_string(val));
    }
};

template<typename T>
struct ToInner<T, std::wstring> {
    static std::wstring C(const T& val) {
        return std::move(std::to_wstring(val));
    }
};

template<>
struct ToInner<std::wstring, std::string> {
    static std::string C(const std::wstring& val) {
        static std::vector<char> buffer(2048);
        buffer.clear();
        const wchar_t* wsrc = val.c_str();
        while(*wsrc != 0) {
            if(*wsrc < 0x80) {
                buffer.push_back((char)*wsrc);
            } else if(*wsrc < 0x800) {
                buffer.push_back(((*wsrc >> 6) & 0x1f) | 0xc0);
                buffer.push_back(((*wsrc) & 0x3f) | 0x80);
            } else {
                buffer.push_back(((*wsrc >> 12) & 0xf) | 0xe0);
                buffer.push_back(((*wsrc >> 6) & 0x3f) | 0x80);
                buffer.push_back(((*wsrc) & 0x3f) | 0x80);
            }
            wsrc++;
        }
        std::string str;
        str.insert(str.begin(), buffer.begin(), buffer.end());
        return str;
    }
};

template<>
struct ToInner<std::string, std::wstring> {
    static std::wstring C(const std::string& val) {
        static std::vector<wchar_t> buffer(2048);
        const char* p = val.c_str();
        buffer.clear();
        while(*p != 0) {
            if((*p & 0x80) == 0) {
                buffer.push_back(*p);
                p++;
            } else if((*p & 0xe0) == 0xc0) {
                buffer.push_back((((int)p[0] & 0x1f) << 6) | ((int)p[1] & 0x3f));
                p += 2;
            } else if((*p & 0xf0) == 0xe0) {
                buffer.push_back((((int)p[0] & 0xf) << 12) | (((int)p[1] & 0x3f) << 6) | ((int)p[2] & 0x3f));
                p += 3;
            } else if((*p & 0xf8) == 0xf0) {
                buffer.push_back((((int)p[0] & 0x7) << 18) | (((int)p[1] & 0x3f) << 12) | (((int)p[2] & 0x3f) << 6) | ((int)p[3] & 0x3f));
                p += 4;
            } else
                p++;
        }
        std::wstring str;
        str.insert(str.begin(), buffer.begin(), buffer.end());
        return str;
    }
};

template<typename TOTYPE, typename T>
TOTYPE To(const T& val) {
    return ToInner<T, TOTYPE>::C(val);
}

template<typename TOTYPE, typename T, typename... REST>
std::string To(const char* format, T val, REST... r) {
    char buf[256];
    snprintf(buf, 256, format, val, std::forward<REST>(r)...);
    return std::string(buf);
}

template<typename TOTYPE, typename T, typename... REST>
std::wstring To(const wchar_t* format, T val, REST... r) {
    wchar_t buf[256];
    swprintf(buf, 256, format, val, std::forward<REST>(r)...);
    return std::wstring(buf);
}

#endif
