#ifndef _CONVERT_H_
#define _CONVERT_H_

#include <stdlib.h>
#include <string>

// never use oct
inline int _GetRadix(const char*& str) {
    if(str[0] == '0' && (str[1] == 'X' || str[1] == 'x'))
        return 16;
    return 10;
}

template<int type>
struct Converter {
    static unsigned long long Convert(const char* str) {
        const char* s = str;
        int radix = _GetRadix(s);
        return strtoull(s, nullptr, radix);
    }
};

template<>
struct Converter<2> {
    static double Convert(const char* str) {
        return atof(str);
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

template<typename T>
T To(const char* str) {
    return static_cast<T>(Converter<ConverterType<T>::conv_type>::Convert(str));
};

template<typename T>
T To(const std::string& str) {
    return static_cast<T>(Converter<ConverterType<T>::conv_type>::Convert(str.c_str()));
};

#endif
