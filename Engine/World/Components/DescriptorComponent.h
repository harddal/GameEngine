#pragma once

#include <string>

#include <anax/Component.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <tinyxml2/tinyxml2.h>

#include "Engine/Types.h"
#include "Utility/Utility.h"

enum ENTITY_TYPE
{
    ET_NULL,
    ET_STATIC,
    ET_DYNAMIC,
    ET_PLAYER
};

struct DescriptorComponent : anax::Component
{
    bool isSerializable, isDebug, isAlive, processedDeathAction;

    entityid id;

    std::string name;

    ENTITY_TYPE type;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(name), CEREAL_NVP(type), CEREAL_NVP(isDebug), CEREAL_NVP(isSerializable));
    }

    DescriptorComponent() :
        isSerializable(true), isDebug(false), isAlive(true), processedDeathAction(false), id(0U), name(std::string()), type(ET_NULL) {}
};
