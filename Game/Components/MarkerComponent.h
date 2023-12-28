#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

enum MARKER_TYPE
{
	MT_NULL,
	MT_PLAYER_START,
	MT_FREECAMERA,
    MT_WAYPOINT
};

struct MarkerComponent : anax::Component
{
	MARKER_TYPE type;

	bool hasUpdated = false;

	template <class Archive>
	void serialize(Archive& archive) { archive(CEREAL_NVP(type)); }
};
