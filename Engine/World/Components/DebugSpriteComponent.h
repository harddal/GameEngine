#pragma once

#include <vector>

#include "anax/Component.hpp"

#include "irrlicht.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

struct DebugSpriteComponent : anax::Component
{
	std::string sprite;

	bool isVisible;

	irr::scene::IBillboardSceneNode *node;
	irr::scene::IAnimatedMesh* selectorMesh;
	irr::scene::IAnimatedMeshSceneNode *selectorNode;

	irr::scene::ITriangleSelector *selector;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(sprite));
	}

    DebugSpriteComponent() : isVisible(false), node(nullptr), selectorMesh(nullptr), selectorNode(nullptr), selector(nullptr) {}
};
