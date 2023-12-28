#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct AutoKillComponent : anax::Component
{
    unsigned int lifetime_ms, spawn_encoded;
	
	template <class Archive>
	void serialize(Archive& archive) { archive(CEREAL_NVP(lifetime_ms)); }
};
