#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct InteractionComponent : anax::Component
{
	bool interact = false;
    
	template <class Archive>
	void serialize(Archive& archive) { /*archive(CEREAL_NVP());*/ }
};
