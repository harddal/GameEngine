#include "GameManager.h"

#include <fstream>
#include <cereal/archives/xml.hpp>
#include <spdlog/spdlog.h>

#include "Engine/Resource/FilePaths.h"
#include "Item/ItemDatabase.h"
#include "Utility/Utility.h"

#include "Player/PlayerController.h"
#include "Player/FreeCameraController.h"

GameManager* GameManager::s_Instance = nullptr;

GameManager::GameManager()
{
	if (s_Instance)
	{
		Utility::Error("Pointer to class \'RenderManager\' is invalid");
	}
	s_Instance = this;

	loadConfiguration();

	m_hasGameInitialized = false;
}

void GameManager::init(const std::string &args)
{
	m_hasGameInitialized = true;

	loadConfiguration();

	ImGui::GetIO().MouseDrawCursor = false;

	m_currentGameArguments = args;

	ItemDatabase::Load();

	Engine::Get()->setGameMode();

	PhysicsManager::Get()->createScene();

	WorldManager::Get()->importScene(args);

	g_FreeCameraController = std::make_unique<FreeCameraController>();
	g_FreeCameraController->init();

	g_PlayerController = std::make_unique<PlayerController>();
	g_PlayerController->init();
}

void GameManager::update(float dt, bool editor_mode)
{
	// If not in debug, escape key activates the menu
	
	static auto esc_pressed = false;
	if (InputManager::Get()->getKeyPressOnce(KEY_ESCAPE, &esc_pressed, true) && !editor_mode)
	{
		Engine::Get()->stateManager()->setStatePauseResume(ESID_MENU);
	}

	if (Engine::Get()->isGameDebugFeaturesEnabled() || editor_mode)
	{
		// Export scene
		static auto f5_pressed = false;
		if (InputManager::Get()->getKeyPressOnce(KEY_F5, &f5_pressed))
		{
			WorldManager::Get()->exportScene(_asset_scn(std::string("debug/") + "debug_exported_scene"));
		}

		// Reload scene
		static auto f6_pressed = false;
		if (InputManager::Get()->getKeyPressOnce(KEY_F6, &f6_pressed))
		{
			this->destroy();
			this->init(_asset_scn(std::string("debug/") + "debug_exported_scene"));
		}

		// Toggle debug sprites
		static auto f10_pressed = false;
		if (InputManager::Get()->getKeyPressOnce(KEY_F10, &f10_pressed))
		{
			WorldManager::Get()->renderSystem()->setDebugSpriteVisible(
				!WorldManager::Get()->renderSystem()->isDebugSpriteVisible());
		}
	}

	g_FreeCameraController->update(dt);
	g_PlayerController->update(dt);
}

void GameManager::updateUI(float dt)
{
	
}

void GameManager::destroy()
{
	m_hasGameInitialized = false;

	Engine::Get()->setGameMode(false);

	g_FreeCameraController->destroy();
	g_FreeCameraController.release();
	
	g_PlayerController->destroy();
	g_PlayerController.release();

	WorldManager::Get()->killAllEntities();
	WorldManager::Get()->clearCVars();

	PhysicsManager::Get()->destroyScene();
}

void GameManager::reset()
{
	if (m_hasGameInitialized)
	{
		destroy();
		init(m_currentGameArguments);
	}
}

void GameManager::pause()
{
	Engine::Get()->setGameMode(false);
}

void GameManager::resume()
{
	ImGui::GetIO().MouseDrawCursor = false;
	InputManager::Get()->centerMouse();

	Engine::Get()->setGameMode();
}

void GameManager::loadConfiguration()
{
	try
	{
		std::ifstream ifs_game("config/game.xml");
		cereal::XMLInputArchive game_config(ifs_game);

		game_config(m_configuration);
	}
	catch (cereal::Exception& ex)
	{
		spdlog::warn("Failed to load game configuration: {}, default values used", ex.what());

		m_configuration = GameConfiguration();

		std::ofstream ofs_game("config/game.xml");
		cereal::XMLOutputArchive game_config(ofs_game);

		game_config(m_configuration);
	}
}

void GameManager::saveConfiguration(GameConfiguration& configuration)
{
	std::ofstream ofs_game("config/game.xml");
	cereal::XMLOutputArchive game_config(ofs_game);

	m_configuration = configuration;
	game_config(configuration);
}