#pragma once

#include <string>

#include <anax/Component.hpp>
#include <PxPhysicsAPI.h>
#include <cereal/cereal.hpp>

enum PHYSICS_COLLIDER_TYPE
{
    PCT_PLANE,
    PCT_BOX,
    PCT_SPHERE,
    PCT_CAPSULE,
    PCT_CONVEX,
    PCT_TRIANGLE
};

struct PhysXActorUserData
{
    int id = 0;
};

struct PhysicsComponent : anax::Component
{
    bool kinematic;

    PHYSICS_COLLIDER_TYPE type;

    physx::PxReal density;
    physx::PxTransform* transform;
    physx::PxVec3* dimensions;
    physx::PxRigidDynamic* actor;

	// DEPRECATED
    std::string collisionMesh;

    physx::PxTriangleMesh* triMesh;
    physx::PxShape* triMeshShape;

    template <class Archive>
    void serialize(Archive& archive) { archive(CEREAL_NVP(type), CEREAL_NVP(kinematic), CEREAL_NVP(collisionMesh), CEREAL_NVP(density)); }

    PhysicsComponent() : kinematic(false), type(PCT_BOX), density(1.0f), transform(nullptr), dimensions(nullptr), actor(nullptr), triMesh(nullptr), triMeshShape(nullptr) {}
    // DEBUG: PhysX may handle all alloc/dealloc
    ~PhysicsComponent()
    {
        delete transform;
        delete dimensions;

        transform = nullptr;
        dimensions = nullptr;

        // HACK: Prevents memory leak, may cause crashes in future
        if (type != PHYSICS_COLLIDER_TYPE::PCT_TRIANGLE)
            actor->release();
    }
};
