#include "Engine/World/Components.h"
#include "Game/Components/AutoKillComponent.h"
#include "Game/Components/DamageReceiverComponent.h"
#include "Game/Components/DataComponent.h"
#include "Game/Components/InteractionComponent.h"
#include "Game/Components/ItemComponent.h"
#include "Game/Components/LogicComponent.h"
#include "Game/Components/MarkerComponent.h"
#include "Game/Components/NPCComponent.h"
#include "Game/Components/TriggerZoneComponent.h"
#include "Game/Components/WaterComponent.h"

enum class ENTITY_COMPONENT
{
	AUTOKILL,
	BILLBOARDSPRITE,
	CAMERA,
	CHARACTERCONTROLLER,
	DAMAGERECEIVER,
	DATA,
	DEBUGMESH,
	DEBUGSPRITE,
	DESCRIPTOR,
	INTERACTION,
	ITEM,
	LIGHT,
	LOGIC,
	MARKER,
	MESH,
	NPC,
	PHYSICS,
	PREFAB,
	RENDER,
	SCRIPT,
	SOUND,
	SOUNDLISTENER,
	TRANSFORM,
	TRIGGERZONE
};