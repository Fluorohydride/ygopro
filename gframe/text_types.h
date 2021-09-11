#ifndef TEXT_TYPES_H_
#define TEXT_TYPES_H_
#include <string>
#include <fmt/core.h>
#if FMT_VERSION >= 80000
#include <fmt/xchar.h>
#endif
#include "nonstd/string_view.hpp"
namespace nonstd {
namespace sv_lite {
template<typename T>
inline fmt::basic_string_view<T> to_string_view(const nonstd::basic_string_view<T>& s) {
	return { s.data(), s.size() };
}
}
}
namespace irr {
namespace core {
template<typename T, typename TAlloc>
class string;
template<typename T, typename TAlloc>
inline fmt::basic_string_view<T> to_string_view(const irr::core::string<T, TAlloc>& s) {
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
using path_char = wchar_t;
#else
#ifndef EPRO_TEXT
#define EPRO_TEXT(x) x
#endif
using path_char = char;
#endif // UNICODE
using path_string = std::basic_string<path_char>;
template<typename T>
using basic_string_view = nonstd::basic_string_view<T>;
using path_stringview = basic_string_view<path_char>;
using stringview = basic_string_view<char>;
using wstringview = basic_string_view<wchar_t>;
}
using namespace nonstd::literals;
#endif /* TEXT_TYPES_H_ */