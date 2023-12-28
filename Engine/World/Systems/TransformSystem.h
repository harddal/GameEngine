#pragma once

#include "anax/anax.hpp"

#include "Engine/World/Components.h"

class TransformSystem
    : public anax::System<anax::Requires<DescriptorComponent, TransformComponent>>
{
public:
    void init() {}

    void onEntityAdded(anax::Entity& entity) override;
    void onEntityRemoved(anax::Entity& entity) override;

    void update();
};
