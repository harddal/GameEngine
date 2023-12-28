#pragma once

#include "anax/anax.hpp"
#include "Engine/World/Components.h"

#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 1000.0f

class CameraSystem
    : public anax::System<anax::Requires<DescriptorComponent, TransformComponent, CameraComponent>>
{
public:
    void update();

	void onEntityAdded(anax::Entity& entity) override;
	void onEntityRemoved(anax::Entity& entity) override;
};
