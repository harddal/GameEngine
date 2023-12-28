#include "Engine/World/WorldManager.h"

#include "Engine/Physics/PhysicsManager.h"

#include <string>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tinyxml2.h>

#include "Utility/Utility.h"
#include "Engine/Engine.h"

#include "Editor/SceneInteractionManager.h"

using namespace anax;
using namespace cereal;
using namespace std;
using namespace tinyxml2;

WorldManager* WorldManager::s_Instance = nullptr;

WorldManager::WorldManager()
{
    if (s_Instance) { Utility::Error("Pointer to class \'WorldManager\' is invalid"); }
    s_Instance = this;

    m_entityIDArray.fill(false);

    m_gameWorld.addSystem(m_managerSystem);
    m_gameWorld.addSystem(m_cameraSystem);
    m_gameWorld.addSystem(m_transformSystem);
    m_gameWorld.addSystem(m_renderSystem);
    m_gameWorld.addSystem(m_physicsSystem);
    m_gameWorld.addSystem(m_cctSystem);
    m_gameWorld.addSystem(m_scriptSystem);
    m_gameWorld.addSystem(m_soundSystem);
	m_gameWorld.addSystem(m_gameplaySystem);

    m_physicsSystem.init();
    m_cctSystem.init(PhysicsManager::Get()->scene());

    m_gameWorld.refresh();
}

WorldManager::~WorldManager()
{
    m_gameWorld.removeAllSystems();
    
    delete s_Instance;
}

void WorldManager::update(irr::f32 dt)
{
//#ifndef NDEBUG
//	updateEntityQueues();
//
//	m_managerSystem.update();
//	m_transformSystem.update();
//	m_renderSystem.update();
//	m_cameraSystem.update();
//
//	if (Engine::Get()->isGameMode())
//	{
//		m_scriptSystem.update();
//		m_physicsSystem.update(dt);
//		m_cctSystem.update(dt);
//		m_gameplaySystem.update();
//	}
//
//	m_soundSystem.update();
//#else
	m_worldCurrent = Engine::Get()->GetCounter();

    updateEntityQueues();

	m_managerCurrent = Engine::Get()->GetCounter();
    m_managerSystem.update();
	m_managerTime = Engine::Get()->GetCounter() - m_managerCurrent;

	m_transformCurrent = Engine::Get()->GetCounter();
    m_transformSystem.update();
	m_transformTime = Engine::Get()->GetCounter() - m_transformCurrent;

	m_cameraCurrent = Engine::Get()->GetCounter();
	m_cameraSystem.update();
	m_cameraTime = Engine::Get()->GetCounter() - m_cameraCurrent;

	m_renderCurrent = Engine::Get()->GetCounter();
    m_renderSystem.update();
	m_renderTime = Engine::Get()->GetCounter() - m_renderCurrent;

    if (Engine::Get()->isGameMode())
    {
		m_scriptCurrent = Engine::Get()->GetCounter();
        m_scriptSystem.update();
		m_scriptTime = Engine::Get()->GetCounter() - m_scriptCurrent;

		m_physicsCurrent = Engine::Get()->GetCounter();
        m_physicsSystem.update(dt);
		m_physicsTime = Engine::Get()->GetCounter() - m_physicsCurrent;

        m_cctSystem.update(dt);

		m_gameplayCurrent = Engine::Get()->GetCounter();
		m_gameplaySystem.update();
		m_gameplayTime = Engine::Get()->GetCounter() - m_gameplayCurrent;
    }

	m_soundCurrent = Engine::Get()->GetCounter();
    m_soundSystem.update();
	m_soundTime = Engine::Get()->GetCounter() - m_soundCurrent;

	m_worldTime = Engine::Get()->GetCounter() - m_worldCurrent;
//#endif
}

void WorldManager::updateEntityQueues()
{
    for (auto id : m_killedEntityIDQueue)
    {
        auto& ent = m_managerSystem.getEntityByID(id);
        if (ent.isValid()) { m_gameWorld.killEntity(ent); }
    }
    m_killedEntityIDQueue.clear();

    m_gameWorld.refresh();

    for (auto desc : m_entitySpawnQueue)
    {
        this->deserializeEntity(desc.file, desc.id, desc.preserve_transform, desc.name, 
            desc.position, desc.rotation, desc.scale);
    }
    m_entitySpawnQueue.clear();

    m_gameWorld.refresh();
}

void WorldManager::freeEntityID(entityid id) { if (id < _entity_null_value) { m_entityIDArray[id] = false; } }

bool WorldManager::killEntityByName(const std::string& name)
{
    auto &ent = m_managerSystem.getEntityByName(name);

    if (ent.isValid()) {

        m_killedEntityIDQueue.emplace_back(ent.getComponent<DescriptorComponent>().id);

        //ent.kill();
        // DEBUG If the world refreshes the entity is removed in the frame so other functions that relie on its ID will fail
        //       Not refreshing the world 'waits' until the end of the frame to 'remove' the entity
        //       May cause problems if an entity is 'killed' then accessed before a world refresh
        //world()->refresh();

        return true;
    }

    return false;
}

bool WorldManager::killEntityByID(int id)
{
    if (id >= 0) {
        auto &ent = m_managerSystem.getEntityByID(id);

        if (ent.isValid()) {

            m_killedEntityIDQueue.emplace_back(id);

            //ent.kill();
            // DEBUG If the world refreshes the entity is removed in the frame so other functions that relie on its ID will fail
            //       Not refreshing the world 'waits' until the end of the frame to 'remove' the entity
            //       May cause problems if an entity is 'killed' then accessed before a world refresh
            //world()->refresh();

            return true;
        }

        spdlog::warn("Entity invalid in WorldManager::killEntityByID()");

        return false;
    }

    //log::write("Entity ID out of range in WorldManager::killEntityByID()", LOG_WARNING);
    return false;
}

void WorldManager::killAllEntities()
{
    //_sound->soundEngine()->removeAllSoundSources();
    for (auto entity : m_gameWorld.getEntities()) { m_gameWorld.killEntity(entity); }

    m_gameWorld.refresh();

    // Clears any remaining physx actors, mainly trimeshes
    // DEBUG
    //PhysicsManager::Get()->destroyScene();

    m_entityIDArray.fill(false);
}

unsigned int WorldManager::spawnEntity(const std::string& file, const std::string& name, bool preserve_transform,
    const irr::core::vector3df& position,
    const irr::core::vector3df& rotation,
    const irr::core::vector3df& scale)
{
    EntitySpawnDescriptor entity;

    bool flag = true;
    for (auto i = 0U; i < _entity_null_value; i++) {
        if (!m_entityIDArray[i]) {
            entity.id = i;

            m_entityIDArray[i] = true;
            flag = false;

            break;
        }
    }

    if (flag) {
        spdlog::warn("World reports entity count exceeded entity_null_value " + std::to_string(_entity_null_value));
    }

    entity.load(file, preserve_transform, name, position, rotation, scale);
    m_entitySpawnQueue.emplace_back(entity);

    return entity.id;
}

void WorldManager::exportEntity(anax::Entity& entity, const std::string& file, bool save_transform)
{
	std::ofstream ofs_entity(file);
	XMLOutputArchive archive(ofs_entity);
	
	serializeEntity(entity, archive, save_transform);
}

void WorldManager::importPrefab(const std::string& file)
{
	deserializeEntity(file, _entity_null_value, true);
}

void WorldManager::exportPrefab(const std::string& file)
{
	std::ofstream ofs_entity(file);
	XMLOutputArchive archive(ofs_entity);

	try
	{
		archive.setNextName("prefab");
		archive.startNode();

		//archive(prefab_data);

		archive.finishNode();
	}
	catch (Exception& ex)
	{
		spdlog::warn("Failed to serialize prefab data {0}", ex.what());
	}

	// TODO: Loop through entities in the prefab and serialize them
	// for (entity in prefab)
	//     serializeEntity(entity)
}

void WorldManager::importScene(const std::string& file)
{
	deserializeEntity(file, _entity_null_value, true);

	// Process transform parenting
	auto &entities = world()->getEntities();
	for (auto &entity : entities)
	{
		auto &descriptor = entity.getComponent<DescriptorComponent>();
		auto &transform = entity.getComponent<TransformComponent>();

		// DEBUG TODO: Find all parents and assign them their children by name format 'childName childID parentName parentID' no spaces
	}

	RenderManager::Get()->sceneManager()->setAmbientLight(m_currentSceneDescriptor.ambient_light);
	RenderManager::Get()->swapSkyDomeTexture(m_currentSceneDescriptor.skydome_texture);

	g_currentScene = Utility::FilenameFromPath(file);
}

void WorldManager::exportScene(const std::string& file)
{
	// TODO: Save scene data

	std::ofstream ofs_entity(file);
	XMLOutputArchive archive(ofs_entity);

	try
	{
		archive.setNextName("scene");
		archive.startNode();

		archive(m_currentSceneDescriptor);

		archive.finishNode();
	}
	catch (Exception& ex)
	{
		spdlog::warn("Failed to serialize scene data {0}", ex.what());
	}
	
	for (auto e : m_gameWorld.getEntities()) {
		serializeEntity(e, archive, true);
	}
}

bool WorldManager::getCVarExists(const std::string& name)
{
    for (auto &cvar : m_globalCVarList) {
        if (cvar.name == name) {
            return true;
        }
    }

    return false;
}

std::string WorldManager::getCVarValue(const std::string& name)
{
    for (auto &cvar : m_globalCVarList) {
        if (cvar.name == name) {
            return cvar.value;
        }
    }

    return std::string();
}

void WorldManager::setCVar(const std::string& name, const std::string& value)
{
    for (auto &cvar : m_globalCVarList) {
        if (cvar.name == name) {
            cvar.value = value;
            return;
        }
    }

    m_globalCVarList.emplace_back(GlobalCVar(name, value));
}

void WorldManager::removeCVar(const std::string& name)
{
    for (auto i = 0U; i < m_globalCVarList.size(); i++) {
        if (m_globalCVarList[i].name == name) {
            m_globalCVarList.erase(m_globalCVarList.begin() + i);
        }
    }
}

void WorldManager::clearCVars()
{
    vector<GlobalCVar>().swap(m_globalCVarList);
}
