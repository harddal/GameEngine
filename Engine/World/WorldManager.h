#pragma once

#include <string>

#include <irrlicht.h>
#include <anax/anax.hpp>

#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>

#include "Engine/World/Components.h"
#include "Engine/World/Systems.h"

#include "Engine/Types.h"
#include "Game/GameplaySystem.h"

struct GlobalCVar
{
    std::string name, value;

    GlobalCVar(std::string name, std::string value)
    {
        this->name = name;
        this->value = value;
    }
};

struct EntitySpawnDescriptor
{
    entityid id;

    std::string file;
    std::string name;

    bool preserve_transform;

    irr::core::vector3df position;
    irr::core::vector3df rotation;
    irr::core::vector3df scale;

    EntitySpawnDescriptor() : 
        id(_entity_null_value), preserve_transform(false), position(irr::core::vector3df(0.0f, 0.0f, 0.0f)), rotation(irr::core::vector3df(0.0f, 0.0f, 0.0f)), scale(irr::core::vector3df(1.0f, 1.0f, 1.0f)) {}
    EntitySpawnDescriptor(const std::string& file, bool preserve_transform = false, const std::string& name = "",
        const irr::core::vector3df& position = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& rotation = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale    = irr::core::vector3df(1.0f, 1.0f, 1.0f)) : id(_entity_null_value)
    {
        this->file = file;
        this->preserve_transform = preserve_transform;
        this->name = name;
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    void load(const std::string& file, bool preserve_transform = false, const std::string& name = "",
        const irr::core::vector3df& position = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& rotation = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale    = irr::core::vector3df(1.0f, 1.0f, 1.0f))
    {
        this->file = file;
        this->preserve_transform = preserve_transform;
        this->name = name;
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }
};

struct SceneDescriptor
{
    std::string name, creator, notes, skydome_texture;

    irr::video::SColorf ambient_light;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(name), CEREAL_NVP(creator), CEREAL_NVP(notes), CEREAL_NVP(skydome_texture),
			CEREAL_NVP(ambient_light.r), CEREAL_NVP(ambient_light.g), CEREAL_NVP(ambient_light.b));
	}
	
    SceneDescriptor& operator=(SceneDescriptor desc)
    {
        std::swap(name, desc.name);
		std::swap(creator, desc.creator);
		std::swap(notes, desc.notes);
        std::swap(skydome_texture, desc.skydome_texture);
        std::swap(ambient_light, desc.ambient_light);

        return *this;
    }

    SceneDescriptor()
    {
        skydome_texture = "content/texture/color/black.png";
        ambient_light = irr::video::SColorf(0.5f, 0.5f, 0.5f);
    }
};

class WorldManager
{
public:
    WorldManager& operator=(const WorldManager&) = delete;

    WorldManager();
    ~WorldManager();

    void update(irr::f32 dt);

    void updateEntityQueues();

    void freeEntityID(entityid id);

    bool killEntityByName(const std::string& name);
    bool killEntityByID(int id);
    void killAllEntities();

    unsigned int spawnEntity(const std::string& file, const std::string&  name = "", bool preserve_transform = false,
        const irr::core::vector3df& position = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& rotation = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale    = irr::core::vector3df(1.0f, 1.0f, 1.0f));
	void exportEntity(anax::Entity& entity, const std::string& file, bool save_transform = false);

	// NOIMP
	void importPrefab(const std::string& file);
	// NOIMP
	void exportPrefab(const std::string& file);
	
	void importScene(const std::string& file);
	void exportScene(const std::string& file);

	SceneDescriptor getCurrentSceneDescriptor() const { return m_currentSceneDescriptor; }
	void setCurrentSceneDescriptor(SceneDescriptor& desc) { m_currentSceneDescriptor = desc; }
	
    bool getCVarExists(const std::string& name);
    std::string getCVarValue(const std::string& name);
    void setCVar(const std::string& name, const std::string& value);
    void removeCVar(const std::string& name);
    void clearCVars();

    anax::World* world() { return &m_gameWorld; }

    CameraSystem*    cameraSystem()    { return &m_cameraSystem; }
    ManagerSystem*   managerSystem()   { return &m_managerSystem; }
    PhysicsSystem*   physicsSystem()   { return &m_physicsSystem; }
    CCTSubsystem*    cctSystem()       { return &m_cctSystem; }
    RenderSystem*    renderSystem()    { return &m_renderSystem; }
    ScriptSystem*    scriptSystem()    { return &m_scriptSystem; }
    SoundSystem*     soundSystem()     { return &m_soundSystem; }
    TransformSystem* transformSystem() { return &m_transformSystem; }
	GameplaySystem*  gameplaySystem()  { return &m_gameplaySystem; }

    static WorldManager* Get() { return s_Instance; }

	irr::f32 getWorldTime() { return m_worldTime; }
	irr::f32 getCameraTime() { return m_cameraTime; }
	irr::f32 getManagerTime() { return m_managerTime; }
	irr::f32 getPhysicsTime() { return m_physicsTime; }
	irr::f32 getRenderTime() { return m_renderTime; }
	irr::f32 getScriptTime() { return m_scriptTime; }
	irr::f32 getSoundTime() { return m_soundTime; }
	irr::f32 getTransformTime() { return m_transformTime; }
	irr::f32 getGameplayTime() { return m_gameplayTime; }

protected:
    std::vector<GlobalCVar> m_globalCVarList;

    void serializeEntity(anax::Entity& entity, cereal::XMLOutputArchive& archive, bool save_transform = false);
    entityid deserializeEntity(const std::string& file = "", entityid id = _entity_null_value,
        bool use_saved_transform = false, const std::string& name = "",
        const irr::core::vector3df& position = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& rotation = irr::core::vector3df(0.0f, 0.0f, 0.0f),
        const irr::core::vector3df& scale    = irr::core::vector3df(1.0f, 1.0f, 1.0f));

private:
    static WorldManager* s_Instance;

	SceneDescriptor m_currentSceneDescriptor;
	
    anax::World m_gameWorld;

    std::vector<entityid> m_killedEntityIDQueue;
    std::vector<EntitySpawnDescriptor> m_entitySpawnQueue;

    std::array<bool, _entity_null_value> m_entityIDArray;

    CameraSystem m_cameraSystem;
    ManagerSystem m_managerSystem;
    PhysicsSystem m_physicsSystem;
    CCTSubsystem m_cctSystem;
    RenderSystem m_renderSystem;
    ScriptSystem m_scriptSystem;
    SoundSystem m_soundSystem;
    TransformSystem m_transformSystem;
	GameplaySystem m_gameplaySystem;

	irr::f32 m_worldCurrent, m_worldLast, m_worldTime;
	irr::f32 m_cameraCurrent, m_cameraLast, m_cameraTime;
	irr::f32 m_managerCurrent, m_managerLast, m_managerTime;
	irr::f32 m_physicsCurrent, m_physicsLast, m_physicsTime;
	irr::f32 m_renderCurrent, m_renderLast, m_renderTime;
	irr::f32 m_scriptCurrent, m_scriptLast, m_scriptTime;
	irr::f32 m_soundCurrent, m_soundLast, m_soundTime;
	irr::f32 m_transformCurrent, m_transformLast, m_transformTime;
	irr::f32 m_gameplayCurrent, m_gameplayLast, m_gameplayTime;
};