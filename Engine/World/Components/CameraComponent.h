#pragma once

#include "anax/Component.hpp"

#include "irrlicht.h"

#include "cereal/cereal.hpp"

struct CameraComponent : anax::Component
{
	bool hasInit = false;

    irr::core::vector3df
        offset, target, lookat, neartarget;
	
    irr::scene::ICameraSceneNode* camera;
    irr::scene::ISceneNode* targetNode;
    irr::scene::ISceneNode* nearTargetNode;

	irr::core::vector3df getLookAt() const
	{
		return lookat;
	}
	irr::core::vector3df getLookAtNormalized() const
	{
		auto lan = lookat;
		return lan.normalize();
	}

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(offset.X), CEREAL_NVP(offset.Y), CEREAL_NVP(offset.Z),
                CEREAL_NVP(target.X), CEREAL_NVP(target.Y), CEREAL_NVP(target.Z));
    }

    CameraComponent() : 
        offset(irr::core::vector3df(0, 0, 0)), target(irr::core::vector3df(0, 0, 100)), 
        lookat(irr::core::vector3df(0, 0, 0)), neartarget(irr::core::vector3df(0, 0, 1)),
        camera(nullptr), targetNode(nullptr), nearTargetNode(nullptr) {}
};
