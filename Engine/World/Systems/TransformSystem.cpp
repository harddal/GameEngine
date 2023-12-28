#include "TransformSystem.h"

#include "Engine/Engine.h"
#include "Utility/Utility.h"

using namespace irr::core;

void TransformComponent::addChild(irr::scene::ISceneNode* child_node)
{
	if (child_node)
	{
		auto &child_ent = WorldManager::Get()->managerSystem()->getEntityByID(child_node->getID());
		if (!child_ent.isValid())
		{
			spdlog::warn("Child entity is not valid in TransformComponent::addChild");
			return;
		}

		child_node->updateAbsolutePosition();

		node->addChild(child_node);
		children.push_back(child_node->getID());

		child_ent.getComponent<TransformComponent>().parent = node->getID();

		populateParentChildNamesForExport();

		isParent = true;
	}
}

void TransformComponent::removeChild(irr::scene::ISceneNode* child_node)
{
	if (child_node && children.size() > 0)
	{
		int iter = 0;
		bool found = false;

		for (auto i : children)
		{
			if (i == child_node->getID())
			{
				found = true;
				break;
			}

			iter++;
		}

		if (found)
		{
			children.erase(children.begin() + iter);
		}

		WorldManager::Get()->managerSystem()->getEntityByID(child_node->getID()).getComponent<TransformComponent>().parent = _entity_null_value;

		node->removeChild(child_node);

		if (children.empty())
		{
			isParent = false;
		}
	}
}

void TransformComponent::populateParentChildNamesForExport()
{
	auto &parent_entity = WorldManager::Get()->managerSystem()->getEntityByID(node->getID());

	if (parent_entity.isValid())
	{
		auto &parent_desc = parent_entity.getComponent<DescriptorComponent>();

		// BUG: Changing the players name causes anax to freak out when it can't reference the player by name
		// Might have to make this a special case when loading parent/child lists
		//parent_name = parent_desc.name = parent_desc.name + std::to_string(parent_desc.id);
		// FIX
		if (parent_desc.name == "player" || parent_desc.name == "freecamera")
		{
			parent_name = parent_desc.name;
		}
		else
		{
			parent_name = parent_desc.name = parent_desc.name + std::to_string(parent_desc.id);
		}

		for (auto id : children)
		{
			auto &child_entity = WorldManager::Get()->managerSystem()->getEntityByID(id);

			if (child_entity.isValid())
			{
				auto &child_desc = child_entity.getComponent<DescriptorComponent>();

				// DEBUG: Consider shortening the string for performance, destroys speed for string based entity name searches
				child_desc.name = child_desc.name + std::to_string(child_desc.id) + parent_desc.name + std::to_string(parent_desc.id);
			}
		}
	}
}

void TransformSystem::onEntityAdded(anax::Entity& entity)
{
    auto& transform = entity.getComponent<TransformComponent>();

    transform.node =
        RenderManager::Get()->sceneManager()->addEmptySceneNode();

	transform.node->setID(entity.getComponent<DescriptorComponent>().id);

	transform.initialPosition = transform.position;
	transform.initialRotation = Math::ConstrainAngleVector3(transform.rotation);
	transform.initialScale    = transform.scale;
	
    transform.node->setPosition(transform.position);
    transform.node->setRotation(Math::ConstrainAngleVector3(transform.rotation));
    transform.node->setScale(transform.scale);

    if (transform.isChild) 
    {
		auto &parent_entity = WorldManager::Get()->managerSystem()->getEntityByID(transform.parent);

		if (parent_entity.isValid())
		{
			if (parent_entity.hasComponent<TransformComponent>())
			{
				parent_entity.getComponent<TransformComponent>().addChild(transform.node);
			}
		}
    }
}


void TransformSystem::onEntityRemoved(anax::Entity& entity)
{
	auto &transform = entity.getComponent<TransformComponent>();

	for (auto id : transform.children)
	{
		auto &child_entity = WorldManager::Get()->managerSystem()->getEntityByID(id);
		if (child_entity.isValid())
		{
			transform.removeChild(child_entity.getComponent<TransformComponent>().node);
		}
	}

	transform.children.clear();

    RenderManager::Get()->sceneManager()->addToDeletionQueue(entity.getComponent<TransformComponent>().node);
}

void TransformSystem::update()
{
    auto& entities = getEntities();

    for (auto& entity : entities) 
    {
		auto& transform = entity.getComponent<TransformComponent>();

		// DEBUG: Fixes bug where rotations can lurch but may have unintended side effects
        transform.node->setRotation(Math::ConstrainAngleVector3(transform.node->getRotation()));

        transform.node->updateAbsolutePosition();
    }
}
