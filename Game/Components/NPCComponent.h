#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"
#include "Engine/Types.h"

enum class NPC_AI_STATE
{
    INACTIVE,
    IDLE,
    PATROL,
    ALERT,
    ATTACK,
    CHASE,
    FLEE,
    DEAD
};

struct NPCComponent : anax::Component
{
    bool isAlive;

    float visionRange, chaseRange, attackRange;

    std::string name;

    NPC_AI_STATE state;

    unsigned int start_waypoint_id, current_waypoint_id;
    std::string start_waypoint, current_waypoint;

    float last_attack_time = 0.0, attackDelay, moveSpeed;
	
	template <class Archive>
	void serialize(Archive& archive) { archive(CEREAL_NVP(name), CEREAL_NVP(visionRange), CEREAL_NVP(chaseRange), CEREAL_NVP(attackRange), CEREAL_NVP(moveSpeed), CEREAL_NVP(attackDelay), CEREAL_NVP(state), CEREAL_NVP(start_waypoint), CEREAL_NVP(current_waypoint)); }

    NPCComponent() : isAlive(true), visionRange(5.0f), chaseRange(5.0f), attackRange(1.25f), state(NPC_AI_STATE::IDLE), start_waypoint_id(_entity_null_value), current_waypoint_id(_entity_null_value), attackDelay(2000.0), moveSpeed(0.03f) {}
};
