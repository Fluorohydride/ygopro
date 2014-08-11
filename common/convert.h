#ifndef _CONVERT_H_
#define _CONVERT_H_

#include <stdlib.h>
#include <string>

template<typename T>
struct ConvertType {
    typedef int conv_type;
};

template<>
struct ConvertType<float> {
    typedef double conv_type;
};

template<>
struct ConvertType<double> {
    typedef double conv_type;
};

template<typename T>
T As(const char* str) {
    unsigned long val = strtoul(str, 0, 0);
    return val;
};

//template<>
//double As<double>(const char* str) {
//    double val = atof(str);
//    return val;
//};

template<typename T>
T To(const char* str) {
    return static_cast<T>(As<typename ConvertType<T>::conv_type>(str));
};

template<typename T>
T To(const std::string& str) {
    return static_cast<T>(As<typename ConvertType<T>::conv_type>(str.c_str()));
};

#endif
