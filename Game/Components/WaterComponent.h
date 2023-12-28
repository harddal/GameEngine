#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

struct WaterComponent : anax::Component
{
	template <class Archive>
	void serialize(Archive& archive) { /*archive(CEREAL_NVP());*/ }
};