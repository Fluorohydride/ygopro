#ifndef TEXT_TYPES_H_
#define TEXT_TYPES_H_
#include <string>
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
#endif /* TEXT_TYPES_H_ */