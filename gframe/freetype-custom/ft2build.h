/****************************************************************************
 *
 * ft2build.h
 *
 *   FreeType 2 build and setup macros.
 *
 * Use custom/ft2build.h instead of the default include/freetype/ft2build.h
 * to customize the FreeType 2 build of YGOPro.
 *
 */

#ifndef FT2_BUILD_MY_PLATFORM_H_
#define FT2_BUILD_MY_PLATFORM_H_

#define FT_CONFIG_OPTIONS_H  <ygopro/ftoption.h>
#define FT_CONFIG_MODULES_H  <ygopro/ftmodule.h>

#include <freetype/config/ftheader.h>

#endif /* FT2_BUILD_MY_PLATFORM_H_ */
