#include "Game/GameState.h"

#include "Engine/Engine.h"

#include "Components.h"

void GameState::serializeComponent(anax::Entity& entity, cereal::XMLOutputArchive& archive)
{
	if (entity.hasComponent<AutoKillComponent>())
	{
		archive.setNextName("autokill");
		archive.startNode();

		archive(entity.getComponent<AutoKillComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<DamageReceiverComponent>()) {

		archive.setNextName("damagereceiver");
		archive.startNode();

		archive(entity.getComponent<DamageReceiverComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<DataComponent>()) {

		archive.setNextName("data");
		archive.startNode();

		archive(entity.getComponent<DataComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<InteractionComponent>()) {

		archive.setNextName("interaction");
		archive.startNode();

		archive(entity.getComponent<InteractionComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<ItemComponent>()) {

		archive.setNextName("item");
		archive.startNode();

		archive(entity.getComponent<ItemComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<LogicComponent>()) {

		archive.setNextName("logic");
		archive.startNode();

		archive(entity.getComponent<LogicComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<MarkerComponent>()) {

		archive.setNextName("marker");
		archive.startNode();

		archive(entity.getComponent<MarkerComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<NPCComponent>()) {

		archive.setNextName("npc");
		archive.startNode();

		archive(entity.getComponent<NPCComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<TriggerZoneComponent>()) {

		archive.setNextName("triggerzone");
		archive.startNode();

		archive(entity.getComponent<TriggerZoneComponent>());

		archive.finishNode();
	}

	if (entity.hasComponent<WaterComponent>()) {

		archive.setNextName("water");
		archive.startNode();

		archive(entity.getComponent<WaterComponent>());

		archive.finishNode();
	}
}

void GameState::deserializeComponent(anax::Entity& entity, tinyxml2::XMLNode* component)
{
	if (std::string(component->Value()) == "autokill")
	{
		entity.addComponent<AutoKillComponent>();
	}

	if (std::string(component->Value()) == "damagereceiver") {
		entity.addComponent<DamageReceiverComponent>();
		entity.getComponent<DamageReceiverComponent>().threshold = atoi(component->FirstChild()->FirstChildElement()->GetText());
	}

	if (std::string(component->Value()) == "data") {
		auto& data = entity.addComponent<DataComponent>();

		auto value = component->FirstChild()->FirstChild()->FirstChildElement();
		for (;
			value != nullptr;
			value = value->NextSiblingElement()) {
			data.data.emplace_back(std::string(value->GetText()));
		}
	}

	if (std::string(component->Value()) == "interaction") {
		entity.addComponent<InteractionComponent>();
	}

	if (std::string(component->Value()) == "item") 
	{	
		entity.addComponent<ItemComponent>().item = "null_item";
			
		auto value = component->FirstChild()->FirstChildElement();
		for (;
			value != nullptr;
			value = value->NextSiblingElement())
		{
			if (std::string(value->Name()) == "item")
			{
				entity.getComponent<ItemComponent>().item = std::string(value->GetText());
			}

			if (std::string(value->Name()) == "loadDefaultData")
			{
				entity.getComponent<ItemComponent>().loadDefaultData = Utility::ProcessBoolStatement(std::string(value->GetText()));
			}

			if (std::string(value->Name()) == "data")
			{
				entity.getComponent<ItemComponent>().data = std::string(value->GetText());
			}
		}
	}
	
	if (std::string(component->Value()) == "logic") {
		entity.addComponent<LogicComponent>().isActivated = Utility::ProcessBoolStatement(std::string(component->FirstChild()->FirstChildElement()->GetText()));
		if (component->FirstChild()->FirstChildElement()->NextSiblingElement()->GetText())
		{
			entity.getComponent<LogicComponent>().receiver = component->FirstChild()->FirstChildElement()->NextSiblingElement()->GetText();
		}
		else
		{
			entity.getComponent<LogicComponent>().receiver = std::string();
		}
	}

	if (std::string(component->Value()) == "marker") {
		entity.addComponent<MarkerComponent>();

		auto value = component->FirstChild()->FirstChildElement();
		for (;
			value != nullptr;
			value = value->NextSiblingElement()) {

			if (std::string(value->Name()) == "type") {
				entity.getComponent<MarkerComponent>().type =
					static_cast<MARKER_TYPE>(atoi(value->GetText()));
			}
		}
	}

	if (std::string(component->Value()) == "npc") {
		entity.addComponent<NPCComponent>();

		auto value = component->FirstChild()->FirstChildElement();
		for (;
			value != nullptr;
			value = value->NextSiblingElement()) {

			if (std::string(value->Name()) == "name") {
				if (value->GetText()) {
					entity.getComponent<NPCComponent>().name = value->GetText();
				}
			}

			if (std::string(value->Name()) == "visionRange") {
				entity.getComponent<NPCComponent>().visionRange = atof(value->GetText());
			}

			if (std::string(value->Name()) == "chaseRange") {
				entity.getComponent<NPCComponent>().chaseRange = atof(value->GetText());
			}

			if (std::string(value->Name()) == "attackRange") {
				entity.getComponent<NPCComponent>().attackRange = atof(value->GetText());
			}

			if (std::string(value->Name()) == "attackDelay") {
				entity.getComponent<NPCComponent>().attackDelay = atof(value->GetText());
			}

			if (std::string(value->Name()) == "moveSpeed") {
				entity.getComponent<NPCComponent>().moveSpeed = atof(value->GetText());
			}

			if (std::string(value->Name()) == "state") {
				entity.getComponent<NPCComponent>().state = static_cast<NPC_AI_STATE>(atoi(value->GetText()));
			}

			if (std::string(value->Name()) == "start_waypoint") {
				if (value->GetText()) {
					entity.getComponent<NPCComponent>().start_waypoint = value->GetText();
				}
			}

			if (std::string(value->Name()) == "current_waypoint") {
				if (value->GetText()) {
					entity.getComponent<NPCComponent>().current_waypoint = value->GetText();
				}
			}
		}
	}

	if (std::string(component->Value()) == "triggerzone") {

		entity.addComponent<TriggerZoneComponent>();

		auto value = component->FirstChild()->FirstChildElement();
		for (;
			value != nullptr;
			value = value->NextSiblingElement()) {

			if (std::string(value->Name()) == "trigger_mask") {
				entity.getComponent<TriggerZoneComponent>().mask = static_cast<TRIGGER_ZONE_MASK>(atoi(value->GetText()));
			}
			if (std::string(value->Name()) == "entity") {
				if (entity.getComponent<TriggerZoneComponent>().mask == TRIGGER_ZONE_MASK::ENTITY_NAME) {
					entity.getComponent<TriggerZoneComponent>().entity = value->GetText();
				}
				else {
					entity.getComponent<TriggerZoneComponent>().entity = "null";
				}
			}
			if (std::string(value->Name()) == "triggered_entity") {
				entity.getComponent<TriggerZoneComponent>().triggered_entity = value->GetText();
			}
			if (std::string(value->Name()) == "reset") {
				entity.getComponent<TriggerZoneComponent>().reset = Utility::ProcessBoolStatement(value->GetText());
			}
			if (std::string(value->Name()) == "single_use") {
				entity.getComponent<TriggerZoneComponent>().single_use = Utility::ProcessBoolStatement(value->GetText());
			}
			if (std::string(value->Name()) == "onoff") {
				entity.getComponent<TriggerZoneComponent>().toggle = Utility::ProcessBoolStatement(value->GetText());
			}
			if (std::string(value->Name()) == "invert") {
				entity.getComponent<TriggerZoneComponent>().invert = Utility::ProcessBoolStatement(value->GetText());
			}
		}
	}

	if (std::string(component->Value()) == "water")
	{
		entity.addComponent<WaterComponent>();
	}
}