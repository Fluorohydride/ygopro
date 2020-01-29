#include "custom_skin_enum.h"

namespace ygo {
namespace skin {

#define CLR(val1,val2,val3,val4) irr::video::SColor(val1,val2,val3,val4)

#define DECLR(what,val) irr::video::SColor what##_VAL = val;
#include "custom_skin_enum.inl"
#undef DECLR
#define DECLR(what,val) what##_VAL = val;
void ResetDefaults() {
#include "custom_skin_enum.inl"
}
#undef DECLR
#undef CLR

}

}
