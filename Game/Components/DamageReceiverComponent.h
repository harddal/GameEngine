#pragma once

#include "anax/Component.hpp"

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

enum class DAMAGE_TYPE
{
	DEFAULT
};

struct DamageReceiverComponent : anax::Component
{
	bool invulnerable, buddha;
    
	int
		damageReceived,
		// 100 by default
		threshold;
        
    int health;

    bool receivedDamage = false;

	DAMAGE_TYPE lastReceivedType;

    bool didReceiveDamage() { bool temp = receivedDamage; receivedDamage = false; return temp; }

    template <class Archive>
    void serialize(Archive& archive) { archive(CEREAL_NVP(threshold)); }

	DamageReceiverComponent() : 
		invulnerable(false), buddha(false), damageReceived(0), threshold(100), health(threshold), lastReceivedType(DAMAGE_TYPE::DEFAULT) {}
};
