#pragma once

#include <irrlicht.h>
#include <PxPhysicsAPI.h>

#include <spdlog/spdlog.h>
#include <cereal/cereal.hpp>

#define PHYSX_MAX_THREADS 4U

#define PHYSX_DEFAULT_SCALE 1.0f  // 1 meter standard size
#define PHYSX_DEFAULT_SPEED 9.81f // 9.81 meters per second standard speed

#define PHYSX_DEFAULT_GRAVITY 0.0f, -9.81f, 0.0f
#define PHYSX_DEFAULT_MATERIAL 0.5f, 0.5f, 0.0f // static friction, dynamic friction, restitution

#define IRR_PHYSX_POS_SCALAR 1.0f
#define IRR_PHYSX_DIM_SCALAR .5f

enum RAYCAST_HIT_GROUP
{
    RHG_STATIC     = 1,
    RHG_DYNAMIC    = 2,
	RHG_PREFILTER  = 4,
	RHG_POSTFILTER = 8,
	RHG_ANY_HIT    = 16,
	RHG_NO_BLOCK   = 32,
};

struct RaycastData
{
    bool hit;

    physx::PxRaycastBuffer data;

    RaycastData() : hit(false) {}
};

class UserErrorCallback : public physx::PxErrorCallback
{
public:
    void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
    {
        spdlog::error("PhysX::PxErrorCallback: code {0}, {1}", code , message);
    }
};

struct PhysicsConfiguration
{
    bool gpu;

    int threads;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(gpu), CEREAL_NVP(threads));
    }

    PhysicsConfiguration() :
        gpu(false), threads(0)
    {}
};

class PhysicsManager
{
public:
    PhysicsManager();
    ~PhysicsManager();

    void update(irr::f32 dt);

    void createScene();
    void destroyScene();

	void cookTriangleMeshFromFile(std::string file, physx::PxRigidDynamic* actor, physx::PxVec3 pos = physx::PxVec3(0, 0, 0), physx::PxQuat rot = physx::PxQuat(0, physx::PxVec3(0, 1, 0)), physx::PxVec3 scale = physx::PxVec3(1, 1, 1));
	void cookTriangleMesh(physx::PxRigidDynamic* actor, physx::PxU32 nbVerts, physx::PxU32 triCount, void* verts, void* indices, physx::PxVec3 pos = physx::PxVec3(0, 0, 0), physx::PxQuat rot = physx::PxQuat(0, physx::PxVec3(0, 1, 0)), physx::PxVec3 scale = physx::PxVec3(1, 1, 1));
    void cookTriangleMeshFromMemory(irr::scene::IAnimatedMesh* trimesh, physx::PxRigidDynamic* actor, physx::PxVec3 pos = physx::PxVec3(0, 0, 0), physx::PxQuat rot = physx::PxQuat(0, physx::PxVec3(0, 1, 0)), physx::PxVec3 scale = physx::PxVec3(1, 1, 1));

    RaycastData raycast(irr::core::vector3df origin, irr::core::vector3df direction, double maxDistance, int group = RHG_ANY_HIT);

    physx::PxMaterial* getDefaultMaterial() const { return m_material; }

    physx::PxController* createCCT(physx::PxCapsuleControllerDesc desc);

    physx::PxPhysics*           physics() const { return m_physics; }
    physx::PxScene*             scene()   const { return m_scene; }
    physx::PxCooking*           cooking() const { return m_cooking; }
    physx::PxControllerManager* cct()     const { return m_cctManager; }

    PhysicsConfiguration getConfiguration() const { return m_configuration; }
	void saveConfiguration(PhysicsConfiguration configuration);

    static PhysicsManager* Get() { return s_Instance; }

private:
    static PhysicsManager* s_Instance;

    // 1 works, but 2 or 3 is the best, although it's quite lag inducing
    physx::PxU32 m_substepCount;

    float m_accumulator;
    float m_stepSize;

    PhysicsConfiguration m_configuration;

    physx::PxSceneDesc* m_sceneDesc;

    physx::PxDefaultAllocator m_allocator;
    UserErrorCallback m_errorCallback;

    physx::PxFoundation* m_foundation;
    physx::PxPhysics* m_physics;
    physx::PxDefaultCpuDispatcher* m_dispatcher;
	physx::PxCudaContextManager* m_cuda;
    physx::PxScene* m_scene;
    physx::PxControllerManager* m_cctManager;
    physx::PxCooking* m_cooking;
    physx::PxMaterial* m_material;
	physx::PxPvd*  m_pvd;
	physx::PxPvdTransport* m_transport;

    physx::PxVec3 m_gravity;
    physx::PxTolerancesScale m_scale;

};