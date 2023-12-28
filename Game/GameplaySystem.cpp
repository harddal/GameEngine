#include "GameplaySystem.h"

#include <spdlog/spdlog.h>

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"
#include "Engine/World/WorldManager.h"
#include "Game/Components.h"
#include "Player/PlayerController.h"

#include "Game/Item/ItemDatabase.h"

void GameplaySystem::onEntityAdded(anax::Entity& entity)
{
	if (entity.hasComponent<AutoKillComponent>()) 
	{
		entity.getComponent<AutoKillComponent>().spawn_encoded = Engine::Get()->getCurrentTime();
	}

	if (entity.hasComponent<NPCComponent>())
	{
		auto& npc = entity.getComponent<NPCComponent>();

		if (npc.current_waypoint.empty()) 
		{
			npc.current_waypoint = npc.start_waypoint;
		}
	}
}

void GameplaySystem::onEntityRemoved(anax::Entity& entity)
{
    
}

void GameplaySystem::init()
{
	
}

void GameplaySystem::update()
{
	m_waterZones.clear();

	// Stops zones from overwriting each other if there are more than one, only works for player
	static bool player_in_trigger_zone = false, player_in_water_zone = false;

	auto& entities = getEntities();
	
	for (auto& entity : entities)
	{
		auto& descriptorComponent = entity.getComponent<DescriptorComponent>();

		if (descriptorComponent.type == ET_NULL || descriptorComponent.type == ET_STATIC)
		{
			continue;
		}
		
// ---- POSITION SOUND LISTENER
		if (descriptorComponent.type == ET_PLAYER)
		{
			auto camera = entity.getComponent<CameraComponent>().camera;
			
			SoundManager::Get()->sound()->setListenerPosition(
				camera->getAbsolutePosition(), camera->getTarget(),irr::core::vector3df(), camera->getUpVector());

			continue;
		}

// ---- ENTITY INTERACTION
		if (entity.hasComponent<InteractionComponent>())
		{
			if (entity.hasComponent<ScriptComponent>() && !entity.hasComponent<ItemComponent>())
			{
				if (entity.getComponent<InteractionComponent>().interact)
				{
					auto& script = entity.getComponent<ScriptComponent>();

					if (script.hasOnInteraction)
					{
						ScriptManager::Get()->execute(
							script, script.onPlayerInteractionFunc, descriptorComponent.id);
					}

					entity.getComponent<InteractionComponent>().interact = false;
				}
			}
		}

// ---- ITEM INTERACTION
		if (entity.hasComponent<InteractionComponent>() && entity.hasComponent<ItemComponent>())
		{
			if (entity.getComponent<InteractionComponent>().interact)
			{
				if (g_PlayerController->inventoryController()->pickupItem(
					entity.getComponent<ItemComponent>().item,
					entity.getComponent<DescriptorComponent>().id))
				{
					WorldManager::Get()->killEntityByID(entity.getComponent<DescriptorComponent>().id);

					continue;
				}
			}
		}

// ---- ENTITY DEATH
		if (!descriptorComponent.isAlive && !descriptorComponent.processedDeathAction)
		{
			descriptorComponent.processedDeathAction = true;

			if (entity.hasComponent<ScriptComponent>())
			{
				auto& script = entity.getComponent<ScriptComponent>();

				if (script.hasOnKillEventFunc) 
				{
					ScriptManager::Get()->execute(
						script, script.onKillEventFunc, descriptorComponent.id);
				}

				continue;
			}
		}
		
// ---- AUTOKILL COMPONENT
		if (entity.hasComponent<AutoKillComponent>())
		{
			auto& autokillComponent = entity.getComponent<AutoKillComponent>();
			
			if (Engine::Get()->getCurrentTime() - autokillComponent.spawn_encoded > autokillComponent.lifetime_ms)
			{
				descriptorComponent.isAlive = false;
			}
		}
		
// ---- DAMAGE COMPONENT
		if (entity.hasComponent<DamageReceiverComponent>())
		{
			auto &damageComponent = entity.getComponent<DamageReceiverComponent>();

			if (!damageComponent.invulnerable) 
			{
				damageComponent.health = damageComponent.threshold - damageComponent.damageReceived;
			}
			else 
			{
				damageComponent.health = damageComponent.threshold;
			}

			if (damageComponent.health > damageComponent.threshold) 
			{
				damageComponent.health = damageComponent.threshold;
			}

			if (damageComponent.health <= 0 && !damageComponent.buddha) 
			{
				descriptorComponent.isAlive = false;
			}
		}
		
// ---- LOGIC COMPONENT
		if (entity.hasComponent<LogicComponent>())
		{
			if (entity.hasComponent<ScriptComponent>())
			{
				auto& logicComponent = entity.getComponent<LogicComponent>();

				if (logicComponent.isActivated)
				{
					std::vector<std::string> tokens;
					std::stringstream check1(logicComponent.receiver);
					std::string intermediate;

					while (getline(check1, intermediate, ','))
					{
						tokens.push_back(intermediate);
					}

					for (auto& token : tokens)
					{
						auto& r_ent = WorldManager::Get()->managerSystem()->getEntityByName(token);

						if (r_ent.isValid())
						{
							if (r_ent.hasComponent<LogicComponent>() && r_ent.hasComponent<ScriptComponent>())
							{
								auto& r_ent_logic = r_ent.getComponent<LogicComponent>();
								auto& r_ent_script = r_ent.getComponent<ScriptComponent>();

								r_ent_logic.isActivated = true;

								if (r_ent_script.hasOnLogicEventActivate)
								{
									ScriptManager::Get()->execute(
										r_ent_script, r_ent_script.onLogicEventActivate,
										r_ent.getComponent<DescriptorComponent>().id);
								}

								r_ent_logic.isActivated = false;
							}
							else
							{
								if (r_ent.hasComponent<LightComponent>())
								{
									auto& render = r_ent.getComponent<RenderComponent>();

									render.isVisible = !render.isVisible;
								}
							}
						}

						logicComponent.isActivated = false;
					}
				}
			}
		}
		
// ---- MARKER COMPONENT
		if (entity.hasComponent<MarkerComponent>())
		{
			auto transformComponent = entity.getComponent<TransformComponent>();
			auto& markerComponent   = entity.getComponent<MarkerComponent>();

			switch (markerComponent.type)
			{
			case MT_NULL:
				break;

			case MT_PLAYER_START:
				if (Engine::Get()->isGameMode() && !markerComponent.hasUpdated)
				{
					if (!WorldManager::Get()->managerSystem()->doesEntityExist("player"))
					{
						WorldManager::Get()->spawnEntity(
							_asset_ent("player/player"), "player", false,
							transformComponent.position - irr::core::vector3df(0.0f, PLAYER_HEIGHT, 0.0f),
							transformComponent.rotation);
						
						markerComponent.hasUpdated = true;
					}
					else
					{
						spdlog::debug("MT_PLAYER_START did not spawn a player controller, one already exists");
					}
				}
				else if (markerComponent.hasUpdated)
				{
					auto& playerTransform = WorldManager::Get()->managerSystem()->getEntityByName("player").getComponent<TransformComponent>();
					
					transformComponent.setPosition(playerTransform.getPosition() + irr::core::vector3df(0.0f, PLAYER_HEIGHT, 0.0f));
					transformComponent.setRotation(playerTransform.getRotation());
				}
				break;

			case MT_FREECAMERA:
				if (Engine::Get()->isGameMode() && !markerComponent.hasUpdated)
				{
					if (!WorldManager::Get()->managerSystem()->getEntityByName("freecamera").isValid())
					{
						WorldManager::Get()->spawnEntity(_asset_ent("player/freecamera"), "", false, transformComponent.position,
							transformComponent.rotation);

						markerComponent.hasUpdated = true;
					}
					else
					{
						spdlog::debug("MT_FREECAMERA did not spawn a freecamera controller, one already exists");
					}
				}
				break;

			case MT_WAYPOINT:
				if (entity.hasComponent<DataComponent>())
				{
					auto& dataComponent = entity.getComponent<DataComponent>();

					if (!dataComponent.data.empty())
					{
						auto& next_waypoint = WorldManager::Get()->managerSystem()->getEntityByName(dataComponent.data.back());

						if (next_waypoint.isValid())
						{
							if (Engine::Get()->isEditorMode() || WorldManager::Get()->renderSystem()->isDebugSpriteVisible())
							{
								RenderManager::Get()->renderLine3D(Line3D(
									irr::core::line3df(transformComponent.getPosition(),
										next_waypoint.getComponent<TransformComponent>().getPosition()),
									irr::video::SColor(255, 255, 255, 0)));
							}
						}
					}
				}

				break;
			}
		}

// ---- TRIGGERZONE COMPONENT
		if (entity.hasComponent<TriggerZoneComponent>())
		{
			if (entity.hasComponent<TransformComponent>())
			{
				auto& transformComponent   = entity.getComponent<TransformComponent>();
				auto& triggerzoneComponent = entity.getComponent<TriggerZoneComponent>();

				irr::core::vector3df xyz_min, xyz_max, test_point;
				xyz_min.X = transformComponent.getPosition().X - transformComponent.getScale().X * 0.5f;
				xyz_min.Y = transformComponent.getPosition().Y - transformComponent.getScale().Y * 0.5f;
				xyz_min.Z = transformComponent.getPosition().Z - transformComponent.getScale().Z * 0.5f;
				xyz_max.X = transformComponent.getPosition().X + transformComponent.getScale().X * 0.5f;
				xyz_max.Y = transformComponent.getPosition().Y + transformComponent.getScale().Y * 0.5f;
				xyz_max.Z = transformComponent.getPosition().Z + transformComponent.getScale().Z * 0.5f;


				if (triggerzoneComponent.mask == TRIGGER_ZONE_MASK::PLAYER_ONLY && !player_in_trigger_zone)
				{
					if (WorldManager::Get()->managerSystem()->doesEntityExist("player"))
					{
						auto& player = WorldManager::Get()->managerSystem()->getEntityByName("player");
						if (player.isValid())
						{
							test_point = player.getComponent<TransformComponent>().position;
							triggerzoneComponent.triggered =
								test_point.X <= xyz_max.X && test_point.X >= xyz_min.X &&
								test_point.Y <= xyz_max.Y && test_point.Y >= xyz_min.Y &&
								test_point.Z <= xyz_max.Z && test_point.Z >= xyz_min.Z;

							player_in_trigger_zone = triggerzoneComponent.triggered;
						}
					}
				}
				if (triggerzoneComponent.mask == TRIGGER_ZONE_MASK::ENTITY_NAME) // May not work with multiple zones as if entity is in one but not the other they will overwrite triggerzoneComponent.triggered
				{
					auto& test_entity = WorldManager::Get()->managerSystem()->getEntityByName(triggerzoneComponent.entity);
					
					if (test_entity.isValid())
					{
						test_point = test_entity.getComponent<TransformComponent>().position;
						
						triggerzoneComponent.triggered =
							(test_point.X <= xyz_max.X && test_point.X >= xyz_min.X) &&
							(test_point.Y <= xyz_max.Y && test_point.Y >= xyz_min.Y) &&
							(test_point.Z <= xyz_max.Z && test_point.Z >= xyz_min.Z);
					}
				}

				if (!triggerzoneComponent.triggered && !triggerzoneComponent.single_use)
				{
					triggerzoneComponent.reset = true;
				}
				if (triggerzoneComponent.triggered && !triggerzoneComponent.single_use && !triggerzoneComponent.toggle)
				{
					triggerzoneComponent.reset = true;
				}

				if (triggerzoneComponent.triggered && triggerzoneComponent.reset)
				{
					if (triggerzoneComponent.triggered_entity == "null" || triggerzoneComponent.triggered_entity.empty())
					{
						continue;
					}

					std::vector<std::string> tokens;
					std::stringstream check1(triggerzoneComponent.triggered_entity);
					std::string intermediate;
					while (getline(check1, intermediate, ','))
					{
						tokens.push_back(intermediate);
					}

					for (auto& token : tokens)
					{
						auto& entity = WorldManager::Get()->managerSystem()->getEntityByName(token);
						if (entity.isValid())
						{
							if (entity.hasComponent<LogicComponent>() && entity.hasComponent<ScriptComponent>())
							{
								auto& ent_logic = entity.getComponent<LogicComponent>();
								auto& ent_script = entity.getComponent<ScriptComponent>();
								ent_logic.isActivated = !triggerzoneComponent.invert;

								if (ent_script.hasOnLogicEventActivate)
								{
									ScriptManager::Get()->execute(
										ent_script, ent_script.onLogicEventActivate,
										entity.getComponent<DescriptorComponent>().id);
								}

								ent_logic.isActivated = false;

								triggerzoneComponent.reset = false;
							}
							else if (entity.hasComponent<LightComponent>())
							{
								auto& render = entity.getComponent<RenderComponent>();
								if (triggerzoneComponent.toggle)
								{
									render.isVisible = !render.isVisible;
								}
								if (triggerzoneComponent.invert && !triggerzoneComponent.toggle)
								{
									render.isVisible = false;
								}
								if (!triggerzoneComponent.invert && !triggerzoneComponent.toggle)
								{
									render.isVisible = true;
								}


								triggerzoneComponent.reset = false;
							}
							else if (entity.hasComponent<SoundComponent>())
							{
								auto& sound = entity.getComponent<SoundComponent>();

								if (triggerzoneComponent.toggle)
								{
									sound.play(sound.sounds.at(0).name);
								}
								else
								{
									triggerzoneComponent.toggle = true;
									triggerzoneComponent.single_use = true;

									sound.play(sound.sounds.at(0).name);
								}

								triggerzoneComponent.reset = false;
							}
						}
					}
				}
			}
		}

		// --------- NPC COMPONENT
		if (entity.hasComponent<NPCComponent>())
		{
			double current_time = Engine::Get()->getCurrentTime();

			auto& player = WorldManager::Get()->managerSystem()->getEntityByName("player");
			if (player.isValid())
			{
				entityid player_id = player.getComponent<DescriptorComponent>().id;
				auto& player_transform = player.getComponent<TransformComponent>();

				bool processMovement = false;

				auto& desc = entity.getComponent<DescriptorComponent>();
				auto& mesh = entity.getComponent<MeshComponent>();
				auto& npc = entity.getComponent<NPCComponent>();
				auto& transform = entity.getComponent<TransformComponent>();
				auto& sound = entity.getComponent<SoundComponent>();

				irr::core::vector3df move_target = irr::core::vector3df(0.0f, 0.0f, 0.0f);
				float x_move = 0.0f, y_move = 0.0f, z_move = 0.0f;

				if (!npc.start_waypoint.empty())
				{
					auto& start = WorldManager::Get()->managerSystem()->getEntityByName(npc.start_waypoint);
					auto& current = WorldManager::Get()->managerSystem()->getEntityByName(npc.current_waypoint);

					if (start.isValid())
					{
						npc.start_waypoint_id = start.getComponent<DescriptorComponent>().id;
					}
					else
					{
						npc.start_waypoint_id = _entity_null_value;
					}

					if (current.isValid())
					{
						npc.current_waypoint_id = current.getComponent<DescriptorComponent>().id;
					}
					else
					{
						npc.current_waypoint_id = _entity_null_value;
					}
				}

				//if (player.isValid())
				//{
				//	if (Math::Stable_3D_Distance(transform.getPosition(),
				//	                             player.getComponent<TransformComponent>().getPosition()) < npc.
				//		attackRange)
				//	{
				//		// Currently the raycast can accidently get blocked by an equipped weapon, debug render stuff, etc...
				//		//auto result = _render->raycastWorldPosition(player.getComponent<CameraComponent>().camera->getPosition(), transform.getPosition());

				//		//if (result.node) {
				//		//if (static_cast<unsigned int>(result.node->getID()) == desc.id) {
				//		npc.state = NPC_AI_STATE::ATTACK;
				//		//}
				//		//}
				//	}
				//	else if (npc.state == NPC_AI_STATE::ATTACK)
				//	{
				//		npc.state = NPC_AI_STATE::CHASE;
				//	}
				//}

				if (!desc.isAlive)
				{
					npc.state = NPC_AI_STATE::DEAD;
				}


				switch (entity.getComponent<NPCComponent>().state)
				{
				case NPC_AI_STATE::INACTIVE:
					{
						break;
					}
				case NPC_AI_STATE::IDLE:
					{
						if (mesh.animation_call_back->hasAnimationEnded())
						{
							WorldManager::Get()->renderSystem()->playAnimation(desc.id, "idle");
						}

						if (Math::Stable_3D_Distance(transform.getPosition(),
							player.getComponent<TransformComponent>().getPosition()) < npc.
							visionRange)
						{
							// Currently the raycast can accidently get blocked by an equipped weapon, debug render stuff, etc...
							//auto result = _render->raycastWorldPosition(player.getComponent<CameraComponent>().camera->getPosition(), transform.getPosition());

							//if (result.node) {
							//if (static_cast<unsigned int>(result.node->getID()) == desc.id) {
							npc.state = NPC_AI_STATE::CHASE;
							//}
							//}
						}

						break;
					}
				case NPC_AI_STATE::PATROL:
					{
						if (npc.current_waypoint_id < _entity_null_value)
						{
							auto& waypoint = WorldManager::Get()->managerSystem()->getEntityByID(
								npc.current_waypoint_id);
							auto& waypoint_transform = waypoint.getComponent<TransformComponent>();
							auto& waypoint_data = waypoint.getComponent<DataComponent>();

							if (Math::Stable_3D_Distance(transform.getPosition(), waypoint_transform.getPosition()) <
								0.5f)
							{
								if (!waypoint_data.data.empty())
								{
									npc.current_waypoint = waypoint_data.data.back();
								}
								else
								{
									npc.current_waypoint = std::string();
									npc.current_waypoint_id = _entity_null_value;

									npc.state = NPC_AI_STATE::IDLE;
								}
							}
							else
							{
								waypoint_transform = WorldManager::Get()->managerSystem()->getEntityByName(
									npc.current_waypoint).getComponent<TransformComponent>();
								move_target = waypoint_transform.getPosition();
								z_move += npc.moveSpeed;
								processMovement = true;

								if (mesh.animation_call_back->hasAnimationEnded())
								{
									WorldManager::Get()->renderSystem()->playAnimation(desc.id, "move");
								}
							}
						}

						break;
					}
				case NPC_AI_STATE::ALERT:
					{
						if (mesh.animation_call_back->hasAnimationEnded())
						{
							WorldManager::Get()->renderSystem()->playAnimation(desc.id, "idle");
						}

						break;
					}
				case NPC_AI_STATE::ATTACK:
					{
					if (Math::Stable_3D_Distance(transform.getPosition(),
						player.getComponent<TransformComponent>().getPosition()) > npc.
						attackRange)
					{
						npc.state = NPC_AI_STATE::CHASE;
						
						break;
					}

						move_target = player_transform.getPosition();
						processMovement = true;

						if (current_time - npc.last_attack_time > npc.attackDelay)
						{
							WorldManager::Get()->renderSystem()->playAnimation(desc.id, "melee");
							sound.play("attack");

							this->damageEntity(player_id, 5);

							npc.last_attack_time = current_time;
						}

						break;
					}
				case NPC_AI_STATE::CHASE:
					{
						if (Math::Stable_3D_Distance(transform.getPosition(), player_transform.getPosition()) < npc.attackRange)
						{
							npc.state = NPC_AI_STATE::ATTACK;
						}
						else if (Math::Stable_3D_Distance(transform.getPosition(), player_transform.getPosition()) >
							npc.chaseRange)
						{
							if (npc.current_waypoint_id < _entity_null_value)
							{
								npc.state = NPC_AI_STATE::PATROL;
							}
							else
							{
								npc.state = NPC_AI_STATE::IDLE;
							}
						}
						else
						{
							move_target = player_transform.getPosition();
							z_move += npc.moveSpeed;
							processMovement = true;

							if (mesh.animation_call_back->hasAnimationEnded())
							{
								WorldManager::Get()->renderSystem()->playAnimation(desc.id, "move");
							}
						}

						break;
					}
				case NPC_AI_STATE::FLEE:
					{
						break;
					}
				case NPC_AI_STATE::DEAD:
					{
						if (npc.isAlive)
						{
							WorldManager::Get()->renderSystem()->playAnimation(desc.id, "die");
							sound.play("die");

							npc.isAlive = false;
						}

						break;
					}
				}

				// Calculate 'Gravity'
				auto raycast_data = PhysicsManager::Get()->raycast(
					transform.getPosition() + irr::core::vector3df(0.0, 0.05, 0.0),
					irr::core::vector3df(0.0, -1.0, 0.0), 0.1);
				if (!raycast_data.hit)
				{
					transform.setPosition(transform.getPosition() + irr::core::vector3df(0.0, -0.025, 0.0));
				}

				if (processMovement)
				{
					// Calculate Y-axis offset to traverse stairs/ramps/uneven ground
					raycast_data = PhysicsManager::Get()->raycast(
						transform.getPosition() + irr::core::vector3df(0.0, 0.5, 0.0),
						irr::core::vector3df(0.0, -1.0, 0.0), 0.49);
					if (raycast_data.hit)
					{
						transform.setPosition(
							transform.getPosition() + irr::core::vector3df(
								0.0, raycast_data.data.getAnyHit(0).position.y - transform.position.Y, 0.0));
					}

					irr::core::vector3df
						origin = transform.getPosition(),
						target = move_target;

					// Calculate the distance to where the NPC wants to go
					float xdis = target.X - origin.X;
					float zdis = target.Z - origin.Z;
					float ydis = target.Y - origin.Y;
					float xzdis = sqrtf(xdis * xdis + zdis * zdis);

					// Calculate where the NPC should be pointing towards
					auto lookat = irr::core::vector3df(rad2deg(-atan2f(ydis, xzdis)), rad2deg(-atan2f(-xdis, zdis)), 0);

					transform.setRotation(irr::core::vector3df(0.0f, lookat.Y, 0.0f));

					float moveDirection = deg2rad(transform.getRotation().Y);

					// Calculate the direction the NPC should walk to reach its target
					auto direction = irr::core::vector3df(
						z_move * sin(moveDirection) + x_move * sin(moveDirection + __pi / 2.0f),
						y_move,
						z_move * cos(moveDirection) + x_move * cos(moveDirection + __pi / 2.0f));

					transform.setPosition(transform.getPosition() + direction);

					processMovement = false;
				}
			}
		}

		if (entity.hasComponent<WaterComponent>() && !player_in_water_zone)
		{
			if (entity.hasComponent<TransformComponent>())
			{
				auto& transformComponent = entity.getComponent<TransformComponent>();

				irr::core::vector3df xyz_min, xyz_max;

				xyz_min.X = transformComponent.getPosition().X - transformComponent.getScale().X * 0.5f;
				xyz_min.Y = transformComponent.getPosition().Y - transformComponent.getScale().Y * 0.5f;
				xyz_min.Z = transformComponent.getPosition().Z - transformComponent.getScale().Z * 0.5f;
				xyz_max.X = transformComponent.getPosition().X + transformComponent.getScale().X * 0.5f;
				xyz_max.Y = transformComponent.getPosition().Y + transformComponent.getScale().Y * 0.5f;
				xyz_max.Z = transformComponent.getPosition().Z + transformComponent.getScale().Z * 0.5f;

				m_waterZones.emplace_back(xyz_min, xyz_max);

				if (WorldManager::Get()->managerSystem()->doesEntityExist("player"))
				{
					auto& player = WorldManager::Get()->managerSystem()->getEntityByName("player");

					if (player.isValid())
					{
						// Get camera position so player isn't swimming until camera is under water
						auto test_point1 = player.getComponent<CameraComponent>().camera->getAbsolutePosition() - irr::core::vector3df(0.0, 0.75, 0.0);
						auto test_point2 = player.getComponent<CameraComponent>().camera->getAbsolutePosition() + irr::core::vector3df(0.0, 0.0, 0.0);

						g_PlayerController->setIsSwimming(
							test_point1.X <= xyz_max.X && test_point1.X >= xyz_min.X &&
							test_point1.Y <= xyz_max.Y && test_point1.Y >= xyz_min.Y &&
							test_point1.Z <= xyz_max.Z && test_point1.Z >= xyz_min.Z);

						g_PlayerController->setIHeadUnderWater(
							test_point2.X <= xyz_max.X && test_point2.X >= xyz_min.X &&
							test_point2.Y <= xyz_max.Y && test_point2.Y >= xyz_min.Y &&
							test_point2.Z <= xyz_max.Z && test_point2.Z >= xyz_min.Z);

						player_in_water_zone = g_PlayerController->isSwimming();
					}
				}
			}
		}
	}

	player_in_trigger_zone = false;
	player_in_water_zone = false;
}

void GameplaySystem::destroy()
{
	
}

void GameplaySystem::damageEntity(entityid id, unsigned int damage, DAMAGE_TYPE type)
{
	auto& entities = getEntities();
	for (auto i = 0U; i < entities.size(); i++)
	{
		if (entities[i].getComponent<DescriptorComponent>().id == id)
		{
			if (entities[i].hasComponent<DamageReceiverComponent>())
			{
				auto& dcomp = entities[i].getComponent<DamageReceiverComponent>();

				dcomp.damageReceived += damage;
				dcomp.lastReceivedType = type;
				dcomp.receivedDamage = true;
				dcomp.health = dcomp.threshold - dcomp.damageReceived;

				return;
			}
		}
	}
}

void GameplaySystem::healEntity(entityid id, unsigned int heal)
{
	auto& entities = getEntities();
	for (auto i = 0U; i < entities.size(); i++)
	{
		if (entities[i].getComponent<DescriptorComponent>().id == id)
		{
			if (entities[i].hasComponent<DamageReceiverComponent>())
			{
				auto& dcomp = entities[i].getComponent<DamageReceiverComponent>();

				dcomp.damageReceived -= heal;

				if (dcomp.damageReceived < 0)
				{
					dcomp.damageReceived = 0;
				}

				return;
			}
		}
	}
}

void GameplaySystem::setInvulnerable(entityid id, bool set)
{
	auto& entities = getEntities();
	for (auto i = 0U; i < entities.size(); i++)
	{
		if (entities[i].getComponent<DescriptorComponent>().id == id)
		{
			entities[i].getComponent<DamageReceiverComponent>().invulnerable = set;
			
			return;
		}
	}
}

void GameplaySystem::setBuddha(entityid id, bool set)
{
	auto& entities = getEntities();
	for (auto i = 0U; i < entities.size(); i++)
	{
		if (entities[i].getComponent<DescriptorComponent>().id == id)
		{
			entities[i].getComponent<DamageReceiverComponent>().buddha = set;
			
			return;
		}
	}
}

void GameplaySystem::interact(entityid receiver)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(receiver);

	if (entity.isValid()) 
	{
		if (entity.hasComponent<InteractionComponent>()) 
		{
			entity.getComponent<InteractionComponent>().interact = true;
		}
	}
}