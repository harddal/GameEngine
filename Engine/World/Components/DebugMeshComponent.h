#pragma once

#pragma once

#include "anax/Component.hpp"

#include "irrlicht.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

struct DebugMeshComponent : anax::Component
{
    std::string mesh, texture;

    bool isVisible;

    irr::scene::IAnimatedMesh* trimesh;
    irr::scene::IAnimatedMeshSceneNode* node;

    irr::scene::ITriangleSelector* selector;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(mesh), CEREAL_NVP(texture));
    }

    DebugMeshComponent() : isVisible(false), trimesh(nullptr), node(nullptr), selector(nullptr) {}
};
