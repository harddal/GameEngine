#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

struct DataComponent : anax::Component
{
    std::vector<std::string> data;

    template <class Archive>
    void serialize(Archive& archive)
    {
	    archive(CEREAL_NVP(data));
    }
};
