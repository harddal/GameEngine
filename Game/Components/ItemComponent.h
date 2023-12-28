#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct ItemComponent : anax::Component
{
	bool loadDefaultData = false;
	
    std::string item;

	std::string data;

    template <class Archive>
    void serialize(Archive& archive)
    {
    	if (data.empty())
    	{
			archive(CEREAL_NVP(item), CEREAL_NVP(loadDefaultData));
    	}
		else
		{
			archive(CEREAL_NVP(item), CEREAL_NVP(loadDefaultData), CEREAL_NVP(data));
		}
    }
};
