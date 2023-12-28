#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct PrefabComponent : anax::Component
{
    bool hasInit = false, isChild = false;

    unsigned int parent_id;
    std::string parent;

    template <class Archive>
    void serialize(Archive& archive) { archive(CEREAL_NVP(isChild), CEREAL_NVP(parent)); }
};
