#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"

struct RenderComponent : anax::Component
{
	bool isVisible;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(isVisible));
    }

    RenderComponent() : isVisible(true) {}
};
