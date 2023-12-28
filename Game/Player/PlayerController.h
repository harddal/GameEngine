#pragma once

#include "PlayerData.h"

#include "anax/anax.hpp"

#include "HUDController.h"
#include "InteractionController.h"
#include "InventoryController.h"

//#define DISABLE_HUD_AND_INV
//#define DISPLAY_PLAYER_STATS

// DEBUG: Temporary, need to implement into controller class
#define PLAYER_HEIGHT 1.75f

class PlayerController
{
public:
    PlayerController() {}

    void init();
    void update(float dt);
    void destroy();

    void pause();
    void resume();
	
    void playFootStepSound(anax::Entity& player, int _time, int _delay);
    void playJumpSound(anax::Entity& player);

    bool isMoving() { return m_isMoving; }

	int getCurrentHealth() { return g_PlayerData.currentHealth; }
	int getMaxHealth() { return WorldManager::Get()->managerSystem()->getEntityByName("player").getComponent<DamageReceiverComponent>().threshold; }

	void setIsWeaponEquipped(bool is = true) { g_PlayerData.isWeaponEquipped = is; }

	PlayerData getPlayerData() { return g_PlayerData; }
	void loadPlayerData(std::string file);
	void savePlayerData(std::string file);

	bool isSwimming() { return m_isSwimming; }
	void setIsSwimming(bool swimming = true) { m_isSwimming = swimming; }
	bool isHeadUnderWater() { return m_isHeadUnderWater; }
	void setIHeadUnderWater(bool under = true) { m_isHeadUnderWater = under; }
	bool isBlocking() { return m_isBlocking; }
	void setIsBlocking (bool blocking = true) { m_isBlocking = blocking; }

	HUDController         *hudController() { return &m_hudController; }
	InteractionController *interactionController() { return &m_interactionController; }
	InventoryController   *inventoryController() { return &m_inventoryController; }

private:
    bool m_isMoving = false, m_firstUpdate = true, m_isSwimming = false, m_isHeadUnderWater = false, m_isBlocking = false;

	HUDController         m_hudController;
	InteractionController m_interactionController;
	InventoryController   m_inventoryController;

};

extern std::unique_ptr<PlayerController> g_PlayerController;
extern PlayerData g_PlayerData;
