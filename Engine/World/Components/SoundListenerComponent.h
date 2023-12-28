#pragma once

#include <string>

#include "anax/Component.hpp"
#include "irrklang.h"

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

using namespace irrklang;

struct SoundListenerComponent : anax::Component
{
	template <class Archive>
	void serialize(Archive& archive) {}
};
