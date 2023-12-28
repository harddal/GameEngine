#include "Engine/World/Systems/ManagerSystem.h"

#include "Engine/World/WorldManager.h"
#include "Engine/Engine.h"
#include "Utility/Utility.h"

using namespace anax;
using namespace std;
using namespace irr;

void ManagerSystem::onEntityAdded(Entity& entity)
{
    auto& descriptorComponent = entity.getComponent<DescriptorComponent>();

    if (descriptorComponent.name.empty()) { descriptorComponent.name = "noname" + to_string(descriptorComponent.id); }
}

void ManagerSystem::onEntityRemoved(Entity& entity)
{
    WorldManager::Get()->freeEntityID(entity.getComponent<DescriptorComponent>().id);
}

void ManagerSystem::update()
{
    auto& entities = getEntities();

    m_entityCount = getEntities().size();

    if (entities.size() >= _entity_null_value)
    {
        Utility::Error("World reports entity count '" + std::to_string(entities.size()) +
            "' exceeded entity_null_value " + std::to_string(_entity_null_value));
    }

    //for (auto i = 0U; i < entities.size(); i++) { auto& descriptor = entities[i].getComponent<DescriptorComponent>(); }
}

entityid ManagerSystem::getIDByName(string name)
{
    for (auto entity : getEntities())
    {
        if (entity.getComponent<DescriptorComponent>().name == name)
        {
            return entity.getComponent<DescriptorComponent>().id;
        }
    }

    return _entity_null_value;
}

string ManagerSystem::getNameByID(entityid id)
{
    if (id >= _entity_null_value) { return string(); }

    for (auto entity : getEntities())
    {
        if (entity.getComponent<DescriptorComponent>().id == id)
        {
            return entity.getComponent<DescriptorComponent>().name;
        }
    }

    return string();
}

Entity& ManagerSystem::getEntityByName(string name)
{
    auto& entities = getEntities();
    for (auto& entity : entities) { if (entity.getComponent<DescriptorComponent>().name == name) { return const_cast<Entity&>(entity); } }

    //spdlog::warn("Invalid entity name or entity doesn't exist: " + name + " in ManagerSystem::getEntityByName()");

    return m_nullEntity;
}

Entity& ManagerSystem::getEntityByID(entityid id)
{
    auto& entities = getEntities();
    for (auto i = 0U; i < entities.size(); i++)
    {
		if (entities[i].isValid())
		{
			if (entities[i].getComponent<DescriptorComponent>().id == id) { return const_cast<Entity&>(entities.at(i)); }
		}
		else
		{
			spdlog::warn("Entity is not valid ID: " + to_string(id) + " in ManagerSystem::getEntityByID()");
			return m_nullEntity;
		}
    }

    spdlog::warn("Invalid entity ID: " + to_string(id) + " in ManagerSystem::getEntityByID()");

    return m_nullEntity;
}

bool ManagerSystem::doesEntityExist(string name)
{
    auto& entities = getEntities();

    for (auto& entity : entities) { if (entity.getComponent<DescriptorComponent>().name == name) { return true; } }

    return false;
}

entityid ManagerSystem::getMostRecentEntityID()
{
	return getEntities().back().getComponent<DescriptorComponent>().id;
}
