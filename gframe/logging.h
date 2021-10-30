#ifndef LOGGING_H
#define LOGGING_H

#include "text_types.h"

namespace ygo {

void ErrorLog(epro::stringview msg);

//Explicitrly get a T parameter to be sure that the function is called only with 2+ arguments
//to avoid it capturing calls that woudl fall back to teh default stringview implementation
template<std::size_t N, typename T, typename...Arg>
inline void ErrorLog(char const (&format)[N], T&& arg1, Arg&&... args) {
	ErrorLog(fmt::format(format, std::forward<T>(arg1), std::forward<Arg>(args)...));
}

}

#endif
