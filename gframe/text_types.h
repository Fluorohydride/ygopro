#ifndef TEXT_TYPES_H_
#define TEXT_TYPES_H_
#include <string>
#include <fmt/core.h>
#include "nonstd/string_view.hpp"
namespace nonstd {
namespace sv_lite {
template<typename T>
inline fmt::basic_string_view<T> to_string_view(const nonstd::basic_string_view<T>& s) {
	return { s.data(), s.size() };
}
}
}
// Double macro to convert the macro-defined int to a character literal
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
namespace epro {
#ifdef UNICODE
#ifndef EPRO_TEXT
#define EPRO_TEXT(x) L##x
#endif
using epro::path_char = wchar_t;
#else
#ifndef EPRO_TEXT
#define EPRO_TEXT(x) x
#endif
using epro::path_char = char;
#endif // UNICODE
using epro::epro::path_string = std::basic_string<epro::path_char>;
using epro::epro::path_stringview = nonstd::basic_string_view<epro::path_char>;
using stringview = nonstd::basic_string_view<char>;
using wstringview = nonstd::basic_string_view<wchar_t>;
}
using namespace nonstd::literals;
#endif /* TEXT_TYPES_H_ */