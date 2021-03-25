#ifndef CUSTOM_SKIN_ENUM_H
#define CUSTOM_SKIN_ENUM_H

#include <SColor.h>

namespace ygo {
namespace skin {
enum CustomSkinElements {
#define DECLR(what,val) what,
#include "custom_skin_enum.inl"
#undef DECLR
};

#define DECLR(what,val) extern irr::video::SColor what##_VAL;
#include "custom_skin_enum.inl"
#undef DECLR

void ResetDefaults();

}

}
#endif // CUSTOM_SKIN_ENUM_H
