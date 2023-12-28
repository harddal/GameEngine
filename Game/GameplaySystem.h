#pragma once

#include "anax/anax.hpp"

#include "Game/Components/DamageReceiverComponent.h"
#include "Engine/World/Components/DescriptorComponent.h"

class GameplaySystem : public anax::System<anax::Requires<DescriptorComponent>>
{
public:
    void onEntityAdded(anax::Entity& entity) override;

    void onEntityRemoved(anax::Entity& entity) override;

	void init();
    void update();
	void destroy();

	void damageEntity(entityid id, unsigned int damage, DAMAGE_TYPE type = DAMAGE_TYPE::DEFAULT);
	void healEntity(entityid id, unsigned int heal);
	void setInvulnerable(entityid id, bool set = true);
	void setBuddha(entityid id, bool set = true);

	std::vector<std::pair<irr::core::vector3df, irr::core::vector3df>> getWaterZones() { return m_waterZones; }

	void interact(entityid receiver);
private:
	irr::f32 m_current, m_last, m_time;

	std::vector<std::pair<irr::core::vector3df, irr::core::vector3df>> m_waterZones;

};
