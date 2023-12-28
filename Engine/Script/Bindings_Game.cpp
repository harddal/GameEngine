#include "Bindings.h"

#include "Engine/Engine.h"

#include "Game/Components.h"
#include "Game/Item/ItemDatabase.h"
#include "Game/Player/InventoryController.h"
#include "Game/Player/PlayerController.h"

bool AS_GetCVarExists(std::string name)
{
	return WorldManager::Get()->getCVarExists(name);
}
std::string AS_GetCVarValue(std::string name)
{
	return WorldManager::Get()->getCVarValue(name);
}
void AS_SetCVar(std::string name, std::string value)
{
	WorldManager::Get()->setCVar(name, value);
}
void AS_RemoveCVar(std::string name)
{
	WorldManager::Get()->removeCVar(name);
}
void AS_ClearCVars()
{
	WorldManager::Get()->clearCVars();
}

static void AS_ExecuteCommand(std::string cmd)
{
	// NOIMP
}

void AS_ActivateLogicComponent(int e, bool b)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return;
	}

	if (entity.hasComponent<LogicComponent>()) 
	{
		entity.getComponent<LogicComponent>().isActivated = b;
	}
}

bool AS_IsLogicComponentActivated(int e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return false;
	}

	if (entity.hasComponent<LogicComponent>()) 
	{
		return entity.getComponent<LogicComponent>().isActivated;
	}

	return false;
}

static void AS_DamageEntity(int e, int d)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return;
	}

	WorldManager::Get()->gameplaySystem()->damageEntity(e, d);

}

static void AS_HealEntity(int e, int d)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return;
	}

	WorldManager::Get()->gameplaySystem()->healEntity(e, d);
}

void as_interact(entityid e)
{
	WorldManager::Get()->gameplaySystem()->interact(e);
}

bool as_getPlayerInventoryDisplayState()
{
	return g_PlayerInventoryIsDisplaying;
}

static void DamagePlayer(int d)
{
	WorldManager::Get()->gameplaySystem()->damageEntity(WorldManager::Get()->managerSystem()->getIDByName("player"), d);
}

static void HealPlayer(int d)
{
	WorldManager::Get()->gameplaySystem()->healEntity(WorldManager::Get()->managerSystem()->getIDByName("player"), d);
}

void player_additemtoinv(int id)
{
	g_PlayerController->inventoryController()->pickupItem(id);
}
void player_additemtoinvs(std::string item)
{
	g_PlayerController->inventoryController()->pickupItem(ItemDatabase::GetItemByName(item).id);
}

bool as_isPlayerMoving()
{
	return g_PlayerController->isMoving();
}

void player_additemtoinvEnt(int id, int entid)
{
	g_PlayerController->inventoryController()->pickupItem(id, entid);
}
void player_additemtoinvsEnt(std::string item, int entid)
{
	g_PlayerController->inventoryController()->pickupItem(ItemDatabase::GetItemByName(item).id, entid);
}

void player_dropiteminv(int id)
{
	g_PlayerController->inventoryController()->dropItem(id);
}
void player_removeitemtoinv(int id)
{
	g_PlayerController->inventoryController()->removeItem(id);
}

int GetPlayerHealth()
{
	return g_PlayerController->getCurrentHealth();
}
int GetPlayerMaxHealth()
{
	return g_PlayerController->getMaxHealth();
}

void player_removeCurrentActivatedItem()
{
	g_PlayerController->inventoryController()->removeCurrentActivatedItem();
}

bool player_hasitem(int id)
{
	return g_PlayerController->inventoryController()->getFirstItemCopyOfType(id).id == id;
}
bool player_hasitemstr(std::string name)
{
	return g_PlayerController->inventoryController()->getFirstItemCopyOfType(name).name == name;
}

bool player_isswimming()
{
	return g_PlayerController->isSwimming();
}
bool player_isheadunderwater()
{
	return g_PlayerController->isHeadUnderWater();
}

bool player_isblocking()
{
	return g_PlayerController->isBlocking();
}
void player_setblocking(bool blocking)
{
	g_PlayerController->setIsBlocking(blocking);
}

std::string player_getItemDataEquipped2H(std::string key)
{
	/*if (g_PlayerController->inventoryController()->getItemTwoHand().id < ITEM_NULL_ID) {
		switch (data_slot) {
		case 1:
			return g_PlayerController->inventoryController()->getItemTwoHand().data1;
		case 2:
			return g_PlayerController->inventoryController()->getItemTwoHand().data2;
		case 3:
			return g_PlayerController->inventoryController()->getItemTwoHand().data3;
		case 4:
			return g_PlayerController->inventoryController()->getItemTwoHand().data4;
		default:
			return std::string();
		}
	}*/

	if (g_PlayerController->inventoryController()->getItemTwoHand().id < ITEM_NULL_ID) 
	{
		return g_PlayerController->inventoryController()->getItemTwoHand().data;
	}

	return std::string();
}

void player_setItemDataEquipped2H(std::string key, std::string value)
{
	/*if (g_PlayerController->inventoryController()->getItemTwoHand().id < ITEM_NULL_ID) {
		switch (data_slot) {
		case 1:
			g_PlayerController->inventoryController()->getItemTwoHand().data1 = value;
			break;
		case 2:
			g_PlayerController->inventoryController()->getItemTwoHand().data2 = value;
			break;
		case 3:
			g_PlayerController->inventoryController()->getItemTwoHand().data3 = value;
			break;
		case 4:
			g_PlayerController->inventoryController()->getItemTwoHand().data4 = value;
			break;
		default:
			break;
		}
	}*/

	if (g_PlayerController->inventoryController()->getItemTwoHand().id < ITEM_NULL_ID)
	{
		g_PlayerController->inventoryController()->getItemTwoHand().data = value;
	}
}

void as_lockplayerforinput(bool lock)
{
	g_LockPlayerForInput = lock;
}
bool as_isplayerlocked()
{
	return g_LockPlayerForInput;
}

void ScriptBindings::RegisterGame(asIScriptEngine* engine)
{
	engine->SetDefaultNamespace("game");
	{
		engine->RegisterGlobalFunction("void damage(int, int)", asFUNCTION(AS_DamageEntity), asCALL_CDECL);
		engine->RegisterGlobalFunction("void heal(int, int)", asFUNCTION(AS_HealEntity), asCALL_CDECL);
		
		engine->RegisterGlobalFunction("void activateLogicEvent(int, bool)", asFUNCTION(AS_ActivateLogicComponent), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool isLogicEventActive(int)", asFUNCTION(AS_IsLogicComponentActivated), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("player");
	{
		engine->RegisterGlobalFunction("bool isInventoryDisplayed()", asFUNCTION(as_getPlayerInventoryDisplayState), asCALL_CDECL);

		engine->RegisterGlobalFunction("bool isLocked()", asFUNCTION(as_isplayerlocked), asCALL_CDECL);
		engine->RegisterGlobalFunction("void lock(bool)", asFUNCTION(as_lockplayerforinput), asCALL_CDECL);

		engine->RegisterGlobalFunction("bool isMoving()", asFUNCTION(as_isPlayerMoving), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool isSwimming()", asFUNCTION(player_isswimming), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool isHeadUnderWater()", asFUNCTION(player_isheadunderwater), asCALL_CDECL);

		engine->RegisterGlobalFunction("bool isBlocking()", asFUNCTION(player_isblocking), asCALL_CDECL);
		engine->RegisterGlobalFunction("void blocking(bool)", asFUNCTION(player_setblocking), asCALL_CDECL);

		engine->RegisterGlobalFunction("int health()", asFUNCTION(GetPlayerHealth), asCALL_CDECL);
		engine->RegisterGlobalFunction("int max_health()", asFUNCTION(GetPlayerMaxHealth), asCALL_CDECL);

		engine->RegisterGlobalFunction("void damage(int)", asFUNCTION(DamagePlayer), asCALL_CDECL);
		engine->RegisterGlobalFunction("void heal(int)", asFUNCTION(HealPlayer), asCALL_CDECL);
		
		engine->RegisterGlobalFunction("void give(int)", asFUNCTION(player_additemtoinv), asCALL_CDECL);
		engine->RegisterGlobalFunction("void give(string)", asFUNCTION(player_additemtoinvs), asCALL_CDECL);

		engine->RegisterGlobalFunction("void give(int, int)", asFUNCTION(player_additemtoinvEnt), asCALL_CDECL);
		engine->RegisterGlobalFunction("void give(string, int)", asFUNCTION(player_additemtoinvsEnt), asCALL_CDECL);

		engine->RegisterGlobalFunction("void drop(int)", asFUNCTION(player_dropiteminv), asCALL_CDECL);
		engine->RegisterGlobalFunction("void remove(int)", asFUNCTION(player_removeitemtoinv), asCALL_CDECL);

		engine->RegisterGlobalFunction("bool has(int)", asFUNCTION(player_hasitem), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool has(string)", asFUNCTION(player_hasitemstr), asCALL_CDECL);

		engine->RegisterGlobalFunction("void removeActivatedItem()", asFUNCTION(player_removeCurrentActivatedItem), asCALL_CDECL);

		engine->RegisterGlobalFunction("string getEquippedItemData(string)", asFUNCTION(player_getItemDataEquipped2H), asCALL_CDECL);
		engine->RegisterGlobalFunction("void setEquippedItemData(string, string)", asFUNCTION(player_setItemDataEquipped2H), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("cvar");
	{
		engine->RegisterGlobalFunction("bool exists(string)", asFUNCTION(AS_GetCVarExists), asCALL_CDECL);
		engine->RegisterGlobalFunction("string get(string)", asFUNCTION(AS_GetCVarValue), asCALL_CDECL);
		engine->RegisterGlobalFunction("void set(string, string)", asFUNCTION(AS_SetCVar), asCALL_CDECL);
		engine->RegisterGlobalFunction("void remove(string)", asFUNCTION(AS_RemoveCVar), asCALL_CDECL);
		engine->RegisterGlobalFunction("void remove_all()", asFUNCTION(AS_ClearCVars), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("console");
	{
		engine->RegisterGlobalFunction("void execute(string)", asFUNCTION(AS_ExecuteCommand), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("");
}
