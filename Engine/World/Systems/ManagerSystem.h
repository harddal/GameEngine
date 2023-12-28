#pragma once

#include "anax/anax.hpp"

#include "Engine/World/Components.h"

class ManagerSystem : public anax::System<anax::Requires<DescriptorComponent>>
{
public:
    void onEntityAdded(anax::Entity& entity) override;

    void onEntityRemoved(anax::Entity& entity) override;

    void update();

    entityid getIDByName(std::string name);
    std::string getNameByID(entityid id);

    anax::Entity& getEntityByName(std::string name);
    anax::Entity& getEntityByID(entityid id);

    bool doesEntityExist(std::string name);

	entityid getMostRecentEntityID();

private:
    anax::Entity m_nullEntity;

    int m_entityCount;
};
