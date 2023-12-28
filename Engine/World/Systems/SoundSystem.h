#pragma once

#include "anax/anax.hpp"

#include "Engine/World/Components.h"

class SoundSystem : public anax::System<anax::Requires<DescriptorComponent, SoundComponent>>
{
public:
    void onEntityAdded(anax::Entity& entity) override;
    void onEntityRemoved(anax::Entity& entity) override;

    void update();
};
