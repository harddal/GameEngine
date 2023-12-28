#pragma once

#include <vector>

#include "anax/Component.hpp"

#include "irrlicht.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

class AnimationCallback;

enum TRIANGLE_PICKING_FLAG
{
	TPF_NOT_PICKABLE = 0,
	TPF_PICKABLE = 1
};

struct sAnimationData
{
	bool loop;
	std::string name;
	irr::core::vector2di frames;

    sAnimationData() : loop(false) {}
    sAnimationData(std::string name, unsigned int start, unsigned int end, bool loop = false)
    {
        this->loop = loop;
        this->name = name;
        frames = irr::core::vector2di(start, end);
    }
};

struct MeshComponent : anax::Component
{
	std::string mesh;

    bool
        isPrimitive,
        isVisible,
		isAnimated,
		receiveShadows,
		castShadows,
		transparent,
        disableZDraw,
		disableDeferredRendering;

	irr::s32 fps;

	irr::u32 currentFrame;
	std::vector<sAnimationData> animationList;
    sAnimationData lastPlayedAnimation;

	std::shared_ptr<AnimationCallback> animation_call_back;

	std::vector<std::string> textures;

    irr::scene::IAnimatedMesh* trimesh;
	irr::scene::IAnimatedMeshSceneNode* node;

	irr::scene::ITriangleSelector* selector;

    irr::video::E_MATERIAL_TYPE renderMaterial;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(mesh), CEREAL_NVP(textures), CEREAL_NVP(isVisible), CEREAL_NVP(isAnimated), 
			CEREAL_NVP(receiveShadows), CEREAL_NVP(castShadows), CEREAL_NVP(transparent), CEREAL_NVP(disableZDraw), 
			CEREAL_NVP(disableDeferredRendering), CEREAL_NVP(renderMaterial));
	}

    MeshComponent() : 
        isPrimitive(false), isVisible(false), isAnimated(false), receiveShadows(false), castShadows(false),
		transparent(false), disableZDraw(false), disableDeferredRendering(false),
        fps(60), currentFrame(0), trimesh(nullptr), node(nullptr), selector(nullptr), 
        renderMaterial(irr::video::EMT_SOLID) {}
};
