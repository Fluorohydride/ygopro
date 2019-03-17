#include "CConfigMap.h"

core::stringc CConfigMap::getConfig( const core::stringc& name )
{
    core::map<core::stringc,core::stringc>::Node* node = Map.find(name);

    if ( node == 0 )
        return core::stringc();

    return node->getValue();
}

s32 CConfigMap::getConfigAsInt( const core::stringc& name )
{
    core::stringc cfg = getConfig(name);

    if ( cfg.size() == 0 )
        return 0;

    s32 x = 0;
    sscanf( cfg.c_str(), "%d", &x );

    return x;
}

f32 CConfigMap::getConfigAsFloat( const core::stringc& name )
{
    core::stringc cfg = getConfig(name);

    if ( cfg.size() == 0 )
        return 0;

    f32 x = 0.0f;
    sscanf( cfg.c_str(), "%f", &x );

    return x;
}


core::vector2df CConfigMap::getConfigAsVector2df( const core::stringc& name )
{
    core::stringc cfg = getConfig(name);

    if ( cfg.size() == 0 )
        return core::vector2df(0,0);

    core::vector2df vec;

    sscanf( cfg.c_str(), "%f , %f", &vec.X, &vec.Y );

    return vec;
}

core::vector3df CConfigMap::getConfigAsVector3df( const core::stringc& name )
{
    core::stringc cfg = getConfig(name);

    if ( cfg.size() == 0 )
        return core::vector3df(0,0,0);

    core::vector3df vec;

    sscanf( cfg.c_str(), "%f , %f , %f", &vec.X, &vec.Y, &vec.Z );

    return vec;
}

bool CConfigMap::hasConfig( const core::stringc& name )
{
    core::map<core::stringc,core::stringc>::Node* node = Map.find(name);

    return ( node != 0 );
}

void CConfigMap::setConfig( const core::stringc& name, const core::stringc& value )
{
    Map.set(name,value);
}

