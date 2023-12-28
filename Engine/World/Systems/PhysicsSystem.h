#pragma once

#include <iostream>
#include <string>
#include <thread>

#include "anax/anax.hpp"

#include "PxPhysicsAPI.h"

#include "Engine/World/Components.h"

class GameEngine;
class PhysicsManager;

struct sRaycastData;

enum class COLGROUP
{
    DEFAULT,
    STATIC,
    DYNAMIC,
    KINEMATIC,
    PLAYER,
    RAYCAST
};

class CCTSubsystem
    : public anax::System<anax::Requires<DescriptorComponent, TransformComponent, CCTComponent>>
{
public:
    void onEntityAdded(anax::Entity& entity) override;

    void onEntityRemoved(anax::Entity& entity) override;

    void init(physx::PxScene* scene);

    void update(float dt);

    void cleanup();

private:
    physx::PxCapsuleControllerDesc m_capsuleDesc;
};

class PhysicsSystem
    : public anax::System<anax::Requires<DescriptorComponent, TransformComponent, PhysicsComponent, MeshComponent>>
{
public:
    struct RaycastListener
    {
        virtual void onRaycastHit(std::shared_ptr<GameEngine> engine, sRaycastData* hit) = 0;

        virtual std::string getName() = 0;
    };

    void addListener(RaycastListener* listener) { m_raycastListeners.push_back(listener); }

    void removeListener(RaycastListener* listener)
    {
        m_raycastListeners.erase(std::remove(
                                     m_raycastListeners.begin(),
                                     m_raycastListeners.end(), listener),
									 m_raycastListeners.end());
    }

    void onEntityAdded(anax::Entity& entity) override;

    void onEntityRemoved(anax::Entity& entity) override;

    void init();

    void update(float dt);

    void cleanup();

	// DEPRECATED, NEEDS ALTERNATIVE
    void applyForce(std::string name, physx::PxVec3 force);

    // Returns null vector if actor/pose does not exist
    irr::core::vector3df getKinematicActorPosition(std::string name);
    void setKinematicActorPosition(std::string name, irr::core::vector3df position);

    physx::PxMaterial* getDefaultMaterial()
    {
		return m_defaultMaterial;
    }
    
private:
    std::vector<RaycastListener*> m_raycastListeners;

    bool hasRaycasted = false;

	physx::PxMaterial* m_defaultMaterial;
};
