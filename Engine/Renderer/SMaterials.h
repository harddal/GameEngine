#ifndef SMATERIALS_H_INCLUDED
#define SMATERIALS_H_INCLUDED

#include <irrlicht.h>

namespace irr
{
	namespace video
	{
		struct SMaterials
		{
			E_MATERIAL_TYPE Solid;
			E_MATERIAL_TYPE TransparentRef;
			E_MATERIAL_TYPE Transparent;
			E_MATERIAL_TYPE TransparentSoft;
			E_MATERIAL_TYPE Normal;
			E_MATERIAL_TYPE NormalAnimated;
			E_MATERIAL_TYPE Parallax;
			E_MATERIAL_TYPE DetailMap;
			E_MATERIAL_TYPE LightPoint;
			E_MATERIAL_TYPE LightSpot;
			E_MATERIAL_TYPE LightDirectional;
			E_MATERIAL_TYPE LightAmbient;
		};
	}
}

#endif
