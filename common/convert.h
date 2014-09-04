#ifndef _CONVERT_H_
#define _CONVERT_H_

template<int type>
struct ConverteImpl {
    static unsigned long long ConvertVal(const char* str) {
        const char* s = str;
        int radix = (str[0] == '0' && (str[1] == 'X' || str[1] == 'x')) ? 16 : 10;
        return strtoull(s, nullptr, radix);
    }
    
    template<typename T>
    static std::string ConvertStr(const T& val) {
        char buf[32];
        sprintf(buf, "%ld", static_cast<long>(val));
        return std::move(std::string(buf));
    }
    
    template<typename T>
    static std::string ConvertStr(const char* format, const T& val) {
        char buf[32];
        sprintf(buf, format, static_cast<long>(val));
        return std::move(std::string(buf));
    }
    
};

template<>
struct ConverteImpl<2> {
    static double ConvertVal(const char* str) {
        return atof(str);
    }
    
    template<typename T>
    static std::string ConvertStr(const T& val) {
        char buf[32];
        sprintf(buf, "%lf", val);
        return std::move(std::string(buf));
    }
    
    template<typename T>
    static std::string ConvertStr(const char* format, const T& val) {
        char buf[32];
        sprintf(buf, format, val);
        return std::move(std::string(buf));
    }
    
};

template<typename T>
struct ConverterType {
    static const int conv_type = 1;
};

template<>
struct ConverterType<float> {
    static const int conv_type = 2;
};

template<>
struct ConverterType<double> {
    static const int conv_type = 2;
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
        return static_cast<TOTYPE>(ConverteImpl<ConverterType<TOTYPE>::conv_type>::ConvertVal(str));
    };
};

template<typename TOTYPE>
struct ToInner<const char*, TOTYPE> {
    static TOTYPE C(const char* str) {
        return static_cast<TOTYPE>(ConverteImpl<ConverterType<TOTYPE>::conv_type>::ConvertVal(str));
    };
};

template<typename TOTYPE>
struct ToInner<std::string, TOTYPE> {
    static TOTYPE C(const std::string& str) {
        return static_cast<TOTYPE>(ConverteImpl<ConverterType<TOTYPE>::conv_type>::ConvertVal(str.c_str()));
    };
};

template<typename T>
struct ToInner<T, std::string> {
    static std::string C(const T& val) {
        return std::move(ConverteImpl<ConverterType<T>::conv_type>::ConvertStr(val));
    }
    
    static std::string C(const char* format, const T& val) {
        return std::move(ConverteImpl<ConverterType<T>::conv_type>::ConvertStr(format, val));
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
    char buf[32];
    sprintf(buf, format, val, r...);
    return std::move(std::string(buf));
}

template<typename T>
struct ConvertResult {
    ConvertResult() = default;
    ConvertResult(ConvertResult<T>&& rt) {
        val = std::move(rt.val);
    }
    
    operator T() {
        return std::move(val);
    }
    
    template<typename TOTYPE>
    ConvertResult<TOTYPE> To() {
        ConvertResult<TOTYPE> ret;
        ret.val = ToInner<T, TOTYPE>::C(val);
        return std::move(ret);
    }
    
    template<typename TOTYPE>
    ConvertResult<TOTYPE> To(const char* format) {
        ConvertResult<TOTYPE> ret;
        ret.val = ToInner<T, TOTYPE>::C(val, format);
        return std::move(ret);
    }
    
    T val;
};

template<typename T>
struct ConvertRef {
    ConvertRef() = default;
    ConvertRef(const T& init_val) {
        ref_val = &init_val;
    }
    
    template<typename TOTYPE>
    ConvertResult<TOTYPE> To() {
        ConvertResult<TOTYPE> ret;
        ret.val = ToInner<T, TOTYPE>::C(*ref_val);
        return std::move(ret);
    }
    
    template<typename TOTYPE>
    ConvertResult<TOTYPE> To(const char* format) {
        ConvertResult<TOTYPE> ret;
        ret.val = ToInner<T, TOTYPE>::C(*ref_val, format);
        return std::move(ret);
    }
    
    const T* ref_val;
};

struct Converter {
    template<typename T>
    static ConvertRef<T> From(const T& val) {
        return ConvertRef<T>(val);
    }
};

#endif
