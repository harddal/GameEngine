#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

#include "irrlicht.h"

enum class TRIGGER_ZONE_MASK
{
    NONE,
    PLAYER_ONLY,
    ENTITY_NAME
};

struct TriggerZoneComponent : anax::Component
{
    bool triggered = false, reset = true, single_use = true, toggle = true, invert = false;

    TRIGGER_ZONE_MASK mask;

    std::string entity = "null", triggered_entity = "null";

    template <class Archive>
    void serialize(Archive& archive)
    {
        unsigned int trigger_mask = static_cast<unsigned int>(mask);
        bool onoff = toggle;
        archive(CEREAL_NVP(trigger_mask), CEREAL_NVP(entity), CEREAL_NVP(triggered_entity), CEREAL_NVP(reset), CEREAL_NVP(onoff), CEREAL_NVP(single_use), CEREAL_NVP(invert));
    }
};
