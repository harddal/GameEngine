#pragma once

#include "InventoryData.h"

#define _cct_impulse_scale 50
#define _cct_transform_scale 0.5f

#define _player_interact_distance 1.5f

struct PlayerData
{
    bool isWeaponEquipped = false;

    int
		currentHealth    =  0,
		ammoDisplayValue = -1;

    InventoryData inventoryData;

	PlayerData& operator=(PlayerData data)
	{
		std::swap(isWeaponEquipped, data.isWeaponEquipped);
		std::swap(currentHealth, data.currentHealth);
		std::swap(ammoDisplayValue, data.ammoDisplayValue);
		std::swap(inventoryData, data.inventoryData);

		return *this;
	}
};

extern PlayerData g_PlayerData;