#pragma once

#include <vector>

#include "anax/Component.hpp"

#include "irrlicht.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

struct BillboardSpriteComponent : anax::Component
{
	std::string sprite;

	bool isVisible, animated, loop, finished, destroyOnFinish;

    int split_x, split_y, fps, counter, last_update_time;
    float scale_x, scale_y;

	irr::scene::IBillboardSceneNode *node;
	irr::scene::IAnimatedMesh* selectorMesh;
	irr::scene::IAnimatedMeshSceneNode *selectorNode;

	irr::scene::ITriangleSelector *selector;

    std::vector<irr::video::ITexture*> texture_list;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
            CEREAL_NVP(sprite), CEREAL_NVP(split_x), CEREAL_NVP(split_y), CEREAL_NVP(animated), 
            CEREAL_NVP(fps), CEREAL_NVP(loop), CEREAL_NVP(scale_x), CEREAL_NVP(scale_y), CEREAL_NVP(finished), CEREAL_NVP(destroyOnFinish),
            CEREAL_NVP(counter));
	}

    BillboardSpriteComponent() : 
        isVisible(true), animated(false), loop(false), finished(false),
        split_x(0), split_y(0), fps(30), counter(0),
        last_update_time(0), scale_x(1.0f), scale_y(1.0f),
        node(nullptr), selectorMesh(nullptr), selectorNode(nullptr), selector(nullptr) {}
};
