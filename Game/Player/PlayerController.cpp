#include "PlayerController.h"

#include "Engine/Engine.h"
#include "Engine/World/WorldManager.h"

#include <fstream>

#include "Game/Item/ItemDatabase.h"

#include "PlayerData.h"

std::unique_ptr<PlayerController> g_PlayerController;
PlayerData g_PlayerData;

const float
    g_sensitivity = 0.18f,
    g_topAngle = -89.5f,
    g_bottomAngle = 89.5f,
    g_moveSpeed = 3.0f,
    g_gravity = -9.8f;

bool g_hasFallen = false;
bool g_isOnSurface = false;
bool g_isJumping = false;
bool g_used = false;

float g_jump_offset = 0.0;

int
    g_lastStepTime = 0,
    g_lastJumpTime = 0;

void DisplayPlayerStats()
{
	{
		auto windowWidth = 320, windowHeight = 320;
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		auto& ent_player = WorldManager::Get()->managerSystem()->getEntityByName("player");
		auto& transform = ent_player.getComponent<TransformComponent>();
		auto& camera = ent_player.getComponent<CameraComponent>();

		if (ImGui::Begin("PlayerInfo", reinterpret_cast<bool*>(1),
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("TPX %f", transform.position.X);
			ImGui::Text("TPY %f", transform.position.Y);
			ImGui::Text("TPZ %f", transform.position.Z);
			ImGui::Text("TRX %f", transform.rotation.X);
			ImGui::Text("TRY %f", transform.rotation.Y);
			ImGui::Text("TRZ %f", transform.rotation.Z);
			ImGui::Text("CPX %f", camera.camera->getPosition().X);
			ImGui::Text("CPY %f", camera.camera->getPosition().Y);
			ImGui::Text("CPZ %f", camera.camera->getPosition().Z);
			ImGui::Text("CRX %f", camera.camera->getRotation().X);
			ImGui::Text("CRY %f", camera.camera->getRotation().Y);
			ImGui::Text("CRZ %f", camera.camera->getRotation().Z);
			ImGui::Text("CNX %f", camera.lookat.X);
			ImGui::Text("CNY %f", camera.lookat.Y);
			ImGui::Text("CNZ %f", camera.lookat.Z);

			ImGui::End();
		}
	}
}


void PlayerController::init()
{
	g_PlayerData.inventoryData.size = irr::core::vector2di(9, 3);

#ifndef DISABLE_HUD_AND_INV
	m_hudController.init();
#endif

	m_interactionController.init();

#ifndef DISABLE_HUD_AND_INV
	m_inventoryController.init();
#endif
}

void PlayerController::update(float dt)
{
    auto currentTime = static_cast<int>(Engine::Get()->getCurrentTime());

    anax::Entity& player = WorldManager::Get()->managerSystem()->getEntityByName("player");

    if (!player.isValid()) {
        return;
    }

    auto& transform = player.getComponent<TransformComponent>();
    auto& camera    = player.getComponent<CameraComponent>();
    auto& cct       = player.getComponent<CCTComponent>();
    auto& sound     = player.getComponent<SoundComponent>();
	auto& damage    = player.getComponent<DamageReceiverComponent>();

	// Lock Z rotation
	transform.node->setRotation(irr::core::vector3df(transform.node->getRotation().X, transform.node->getRotation().Y, 0.0f));

    static irr::core::vector3df lastPlayerPosition = transform.getPosition();

    static bool is_crouched = false;

	g_PlayerData.currentHealth = player.getComponent<DamageReceiverComponent>().health;

#ifndef DISABLE_HUD_AND_INV
	m_inventoryController.update(g_PlayerData);
	{
		g_PlayerData.isWeaponEquipped = m_inventoryController.isEquipedTwoHand();
		g_PlayerData.ammoDisplayValue = m_inventoryController.getCurrentWeaponAmmoCount();
	}

	m_hudController.update(g_PlayerData, m_inventoryController.isInventoryDisplaying());
	m_interactionController.update(g_PlayerData);
#endif

    float
        x_move = 0.0,
        y_move = isSwimming() ? 0 : g_gravity,
        z_move = 0.0,
        speed_mod = 0.0,
        fs_delay = 1.25;

    irr::core::vector2df mouseDelta;
	irr::core::vector3df cameraRotation;

	cameraRotation.X = camera.camera->getRotation().X;
	cameraRotation.Y = camera.camera->getRotation().Y;
	cameraRotation.Z = 0.0f;

	// BUG: Prevents the annoying camera mis-rotation
	if (m_firstUpdate)
	{
		InputManager::Get()->centerMouse();
		m_firstUpdate = false;
	}

	if (!g_PlayerInventoryIsDisplaying)
	{
		mouseDelta = InputManager::Get()->getMouseDelta();

		cameraRotation.Y -= mouseDelta.X * g_sensitivity;
		cameraRotation.X -= mouseDelta.Y * g_sensitivity;

		if (cameraRotation.X > g_bottomAngle) {
			cameraRotation.X = g_bottomAngle;
		}
		
		if (cameraRotation.X < g_topAngle) {
			cameraRotation.X = g_topAngle;
		}

		// Camera rotation is independent of transform component
		//camera.camera->setRotation(cameraRotation);
		//transform.setRotation(irr::core::vector3df(transform.getRotation().X, camera.camera->getRotation().Y, 0.0f));
		transform.setRotation(irr::core::vector3df(cameraRotation.X, cameraRotation.Y, 0.0f));

	}

	if (is_crouched)
	{
		speed_mod = -1;
		fs_delay = 1.4f;
	}
	else if (InputManager::Get()->isActionPressed("sprint"))
	{
		speed_mod = 3.0;
		fs_delay = 0.75;
	}
	else
	{
		speed_mod = 0.0;
		fs_delay = 1.1f;
	}

    if (InputManager::Get()->isActionPressed("forward") && !InputManager::Get()->isActionPressed("backward"))
    {
		z_move += g_moveSpeed + speed_mod;
    }
    if (InputManager::Get()->isActionPressed("backward") && !InputManager::Get()->isActionPressed("forward"))
    {
		z_move -= g_moveSpeed + speed_mod;
    }
    if (InputManager::Get()->isActionPressed("strafel") && !InputManager::Get()->isActionPressed("strafer"))
    {
        x_move -= g_moveSpeed + speed_mod;
    }
    if (InputManager::Get()->isActionPressed("strafer") && !InputManager::Get()->isActionPressed("strafel"))
    {
        x_move += g_moveSpeed + speed_mod;
    }

    g_isOnSurface = PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df(2.0, 0.0, 0.0), irr::core::vector3df(0.0, -1.0, 0.0), 1.0, RHG_STATIC).hit;
    if (g_isOnSurface && g_hasFallen)
    {
        g_hasFallen = false;

		if (!isSwimming())
		{
			playJumpSound(player);
		}
    }
    if (!g_isOnSurface)
    {
        g_hasFallen = true;
    }

    if (InputManager::Get()->isActionPressed("jump"))
    {
		if (!isSwimming())
		{
			if (g_isOnSurface && currentTime - g_lastJumpTime > 750 && !is_crouched)
			{
				g_isJumping = true;
				g_lastJumpTime = currentTime;
			}
		}
		else
		{
			// Swim up
			y_move += g_moveSpeed;
		}
    }

    if (InputManager::Get()->isActionPressed("crouch"))
    {
		if (!isSwimming())
		{
			cct.controller->resize(0.5f * IRR_PHYSX_DIM_SCALAR);
			camera.offset.Y = 0.25f;
			is_crouched = true;
		}
		else
		{
			// Swim down
			y_move -= g_moveSpeed;
		}
    }
    else if (is_crouched)
    {
        // Raycast in the center and at each edge of the controller to prevent standing up inside of geometry
        if (!PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df( 0.0f,  0.57f,  0.0f), irr::core::vector3df(0.0, 1.0, 0.0), 0.5).hit &&
            !PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df( 0.25f, 0.57f,  0.0f), irr::core::vector3df(0.0, 1.0, 0.0), 0.5).hit &&
            !PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df(-0.25f, 0.57f,  0.0f), irr::core::vector3df(0.0, 1.0, 0.0), 0.5).hit &&
            !PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df( 0.0f, 0.57f,  0.25f), irr::core::vector3df(0.0, 1.0, 0.0), 0.5).hit &&
            !PhysicsManager::Get()->raycast(transform.getPosition() + irr::core::vector3df( 0.0f, 0.57f, -0.25f), irr::core::vector3df(0.0, 1.0, 0.0), 0.5).hit)
        {
            cct.controller->resize(2.0f * IRR_PHYSX_DIM_SCALAR);
			camera.offset.Y = 0.8f;
            is_crouched = false;
        }
    }

	if (InputManager::Get()->isActionPressed("use") && !g_used)
	{
		auto raycast_data = RenderManager::Get()->raycastWorldPosition(
			RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition(), 
			RenderManager::Get()->sceneManager()->getActiveCamera()->getTarget(), true);

		if (raycast_data.hit)
		{
			if (RenderManager::Get()->getRaycastLength(RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition(), raycast_data) < _player_interact_distance)
			{
				WorldManager::Get()->gameplaySystem()->interact(raycast_data.node->getID());
				g_used = true;
			}
		}
	}
	if (InputManager::Get()->isActionReleased("use"))
	{
		g_used = false;
	}

    if (g_isJumping)
    {
        g_jump_offset += 0.1;

        if (z_move > 0.0)
        {
            z_move += 3.0;
        }

        if (g_jump_offset > 0.8)
        {
            g_jump_offset = 0.0;
            g_isJumping = false;
        }
    }

    float moveDirection = deg2rad(cameraRotation.Y);

    x_move = x_move / _cct_impulse_scale;
    y_move = y_move / _cct_impulse_scale;
    z_move = z_move / _cct_impulse_scale;

    physx::PxControllerFilters filters;
	
	// Old
	cct.displacement = physx::PxVec3(physx::PxVec3(
		z_move * sin(moveDirection) + x_move * sin(moveDirection + __pi / 2.0f),
		y_move + g_jump_offset,
		z_move * cos(moveDirection) + x_move * cos(moveDirection + __pi / 2.0f)));

	cct.controller->move(cct.displacement, 0.01, dt, filters);

	transform.setPosition(irr::core::vector3df(
		static_cast<float>(cct.controller->getPosition().x),
		static_cast<float>(cct.controller->getPosition().y),
		static_cast<float>(cct.controller->getPosition().z)));

    if (transform.getPosition().X > lastPlayerPosition.X || transform.getPosition().Z > lastPlayerPosition.Z ||
        transform.getPosition().X < lastPlayerPosition.X || transform.getPosition().Z < lastPlayerPosition.Z) 
	{
		if (!isSwimming())
		{
			playFootStepSound(player, currentTime, static_cast<int>(500.0f * fs_delay));
		}

        m_isMoving = true;
	}
	else 
	{
		m_isMoving = false;
	}

	lastPlayerPosition = transform.getPosition();

	if (damage.didReceiveDamage()) 
	{
		sound.play("damage" + std::to_string(rand() % 2 + 1));
	}
#ifdef DISPLAY_PLAYER_STATS
	DisplayPlayerStats();
#endif
}

void PlayerController::destroy()
{
#ifndef DISABLE_HUD_AND_INV
	m_hudController.destroy();
#endif

	m_interactionController.destroy();

#ifndef DISABLE_HUD_AND_INV
	m_inventoryController.destroy();
#endif

	m_firstUpdate = true;
}

void PlayerController::pause()
{

}

void PlayerController::resume()
{

}

void PlayerController::playFootStepSound(anax::Entity& player, int _time, int _delay)
{
    auto& transform = player.getComponent<TransformComponent>();

    if (_time - g_lastStepTime > _delay)
    {
        int n = rand() % 4 + 1;

        std::string material =
            Engine::Get()->getMaterialBuilder().getMaterialName(Engine::Get()->getMaterialBuilder().getMaterialFromTexture(
                RenderManager::Get()->getMeshMaterialFromRay(transform.getPosition(), transform.getPosition() + irr::core::vector3df(0, -1, 0))));

		if (material == "invalid")
		{
			return;
		}

        player.getComponent<SoundComponent>().play(material + std::to_string(n));

        g_lastStepTime = _time;
    }
}

void PlayerController::playJumpSound(anax::Entity& player)
{
    auto& transform = player.getComponent<TransformComponent>();

    std::string material =
		Engine::Get()->getMaterialBuilder().getMaterialName(Engine::Get()->getMaterialBuilder().getMaterialFromTexture(
			RenderManager::Get()->getMeshMaterialFromRay(transform.getPosition(), transform.getPosition() + irr::core::vector3df(0, -2, 0))));

    if (material == "invalid")
        return;

    player.getComponent<SoundComponent>().play(material + "jump");
}

void PlayerController::loadPlayerData(std::string file)
{
	tinyxml2::XMLDocument xml;
	if (xml.LoadFile(file.c_str()) != tinyxml2::XML_NO_ERROR) {
		spdlog::error("Failed to load player save data \'" + file + "\' in PlayerController::loadPlayerData()");
		return;
	}

	try {
		auto root = xml.FirstChild()->NextSibling()->FirstChild();

		for (auto dataNode = root->FirstChild()->FirstChildElement();
			dataNode != nullptr;
			dataNode = dataNode->NextSiblingElement()) {
			if (std::string(dataNode->Name()) == "value0") {
				WorldManager::Get()->managerSystem()->getEntityByName("player").getComponent<DamageReceiverComponent>().damageReceived = 100 - atoi(dataNode->GetText());
			}
		}

		auto equipped_item = ItemDatabase::GetItemByName(std::string(root->NextSibling()->FirstChild()->Value()));
		for (auto dataNode = root->NextSibling()->FirstChild()->FirstChild()->FirstChildElement();
			dataNode != nullptr;
			dataNode = dataNode->NextSiblingElement()) {
			if (std::string(dataNode->Name()) == "data1") {
				if (dataNode->GetText()) {
					//equipped_item.data1 = dataNode->GetText();
					continue;
				}
			}
			if (std::string(dataNode->Name()) == "data2") {
				if (dataNode->GetText()) {
					//equipped_item.data2 = dataNode->GetText();
					continue;
				}
			}
			if (std::string(dataNode->Name()) == "data3") {
				if (dataNode->GetText()) {
					//equipped_item.data3 = dataNode->GetText();
					continue;
				}
			}
			if (std::string(dataNode->Name()) == "data4") {
				if (dataNode->GetText()) {
					//equipped_item.data4 = dataNode->GetText();
					continue;
				}
			}
		}

		m_inventoryController.equipTwoHand(equipped_item);

		auto counter = 0U;
		for (
			tinyxml2::XMLNode* itemNode = root->NextSibling()->NextSibling()->FirstChild();
			itemNode != nullptr;
			itemNode = itemNode->NextSibling()) {

			if (std::string(itemNode->Value()) == "null_item") {
				counter++;

				continue;
			}

			auto item = ItemDatabase::GetItemByName(std::string(itemNode->Value()));
			for (auto dataNode = itemNode->FirstChild()->FirstChildElement();
				dataNode != nullptr;
				dataNode = dataNode->NextSiblingElement()) {
				if (std::string(dataNode->Name()) == "data1") {
					if (dataNode->GetText()) {
						//item.data1 = dataNode->GetText();
						continue;
					}
				}
				if (std::string(dataNode->Name()) == "data2") {
					if (dataNode->GetText()) {
						//item.data2 = dataNode->GetText();
						continue;
					}
				}
				if (std::string(dataNode->Name()) == "data3") {
					if (dataNode->GetText()) {
						//item.data3 = dataNode->GetText();
						continue;
					}
				}
				if (std::string(dataNode->Name()) == "data4") {
					if (dataNode->GetText()) {
						//item.data4 = dataNode->GetText();
						continue;
					}
				}
			}

			g_PlayerData.inventoryData.contents.at(counter) = item;

			counter++;
		}
	}
	catch (...) {
		spdlog::error("Failed to deserialize player data \'" + file + "\' in PlayerController::loadPlayerData()");
		return;
	}
}

void PlayerController::savePlayerData(std::string file)
{
	std::ofstream ofs_scene(file);
	cereal::XMLOutputArchive archive(ofs_scene);

	try {
		archive.setNextName("data");
		archive.startNode();
		{
			archive.setNextName("health");
			archive.startNode();
			archive(g_PlayerData.currentHealth);
			archive.finishNode();
		}
		archive.finishNode();

		archive.setNextName("equipped");
		archive.startNode();
		{
			archive.setNextName(m_inventoryController.getItemTwoHand().name.c_str());
			archive.startNode();
			archive(m_inventoryController.getItemTwoHandCopy());
			archive.finishNode();
		}
		archive.finishNode();

		archive.setNextName("inventory");
		archive.startNode();

		for (auto& item : g_PlayerData.inventoryData.contents) {
			archive.setNextName(item.name.c_str());
			archive.startNode();
			archive(item);
			archive.finishNode();
		}

		archive.finishNode();
	}
	catch (...) {
		spdlog::error("Failed to serialize player data in PlayerController::savePlayerData()");
	}
}