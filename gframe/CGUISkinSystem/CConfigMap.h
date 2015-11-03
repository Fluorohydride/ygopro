#ifndef CCONFIGMAP_H_
#define CCONFIGMAP_H_

#include <irrMap.h>
#include <irrString.h>
#include <irrTypes.h>
#include <vector2d.h>
#include <vector3d.h>

using namespace irr;

class CConfigMap
{
public:
    core::stringc getConfig( const core::stringc& name );
    s32 getConfigAsInt( const core::stringc& name );
    f32 getConfigAsFloat( const core::stringc& name );
    core::vector2df getConfigAsVector2df( const core::stringc& name );
    core::vector3df getConfigAsVector3df( const core::stringc& name );

    bool hasConfig( const core::stringc& name );

    void setConfig( const core::stringc& name, const core::stringc& value );

private:
    core::map<core::stringc,core::stringc> Map;
};

#endif


