#ifndef CCONFIGMAP_H_
#define CCONFIGMAP_H_

#include <irrMap.h>
#include <irrString.h>
#include <vector2d.h>
#include <vector3d.h>

class CConfigMap {
public:
	irr::core::stringc getConfig(const irr::core::stringc& name);
	irr::s32 getConfigAsInt(const irr::core::stringc& name);
	irr::f32 getConfigAsFloat(const irr::core::stringc& name);
	irr::core::vector2df getConfigAsVector2df(const irr::core::stringc& name);
	irr::core::vector3df getConfigAsVector3df(const irr::core::stringc& name);

	bool hasConfig(const irr::core::stringc& name);

	void setConfig(const irr::core::stringc& name, const irr::core::stringc& value);

private:
	irr::core::map<irr::core::stringc, irr::core::stringc> Map;
};

#endif


