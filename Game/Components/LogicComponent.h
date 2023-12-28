#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

// NOIMP
struct LogicComponent : anax::Component
{
    bool isActivated = false;

    std::string receiver;

    template <class Archive>
    void serialize(Archive& archive) { archive(CEREAL_NVP(isActivated), CEREAL_NVP(receiver)); }
};
