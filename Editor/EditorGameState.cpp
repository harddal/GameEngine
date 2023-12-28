#include "EditorGameState.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"
#include "SceneInteractionManager.h"

// Is only called once when editor starts, init per entry into game mode is handled by resume()
void EditorGameState::init(std::string args)
{

}

void EditorGameState::update(float dt)
{
	// Exit
	if (InputManager::Get()->isKeyPressed(KEY_ESCAPE))
	{
		Engine::Get()->stateManager()->setStatePauseResume(ESID_EDITOR);
	}

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

	GameManager::Get()->update(dt, true);
}

void EditorGameState::updateUI(float dt)
{
	/*ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(200, 200));
	ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		ImGui::Text("FPS %i\nDEL %i\n\nTRI %i\n\nSCN %i\nENT %i\nPHY %i",
			RenderManager::Get()->driver()->getFPS() + 1,
			static_cast<int>(Engine::Get()->getDeltaTime()) - 1,
			RenderManager::Get()->driver()->getPrimitiveCountDrawn(),
			RenderManager::Get()->sceneManager()->getRootSceneNode()->getChildren().size(),
			WorldManager::Get()->world()->getEntityCount(),
			PhysicsManager::Get()->scene()->getNbActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC)));

		ImGui::End();
	}*/

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(200, 350));
	ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	{
		ImGui::Text(
			"FPS: %i\n"
			"Triangles: %i\n\n"
			"Scene Nodes:   %i\n"
			"Entities:      %i\n"
			"PhysX Objects: %i\n\n",
			RenderManager::Get()->driver()->getFPS(),
			RenderManager::Get()->driver()->getPrimitiveCountDrawn(),
			RenderManager::Get()->sceneManager()->getRootSceneNode()->getChildren().size(),
			WorldManager::Get()->world()->getEntityCount(),
			PhysicsManager::Get()->scene()->getNbActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC)));

		/*ImGui::Text(
		"FPS: %i\n"
		"Triangles: %i\n\n"
		"Scene Nodes:   %i\n"
		"Entities:      %i\n"
		"PhysX Objects: %i\n\n"
		"Total Frame Time:     %i\n"
		"Physics Process Time: %i\n"
		"Render Process Time:  %i\n"
		"World Process Time:   %i\n"
		" - Camera:    %i\n"
		" - Manager:   %i\n"
		" - Physics:   %i\n"
		" - Render:    %i\n"
		" - Script:    %i\n"
		" - Sound:     %i\n"
		" - Transform: %i\n"
		" - Gameplay:  %i",
			RenderManager::Get()->driver()->getFPS(),
			RenderManager::Get()->driver()->getPrimitiveCountDrawn(),
			RenderManager::Get()->sceneManager()->getRootSceneNode()->getChildren().size(),
			WorldManager::Get()->world()->getEntityCount(),
			PhysicsManager::Get()->scene()->getNbActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC)),
			(irr::u32)(Engine::Get()->getDeltaTime()),
			(irr::u32)Engine::Get()->getPhysicsTime(),
			(irr::u32)Engine::Get()->getRenderTime(),
			(irr::u32)WorldManager::Get()->getWorldTime(),
			(irr::u32)WorldManager::Get()->getCameraTime(),
			(irr::u32)WorldManager::Get()->getManagerTime(),
			(irr::u32)WorldManager::Get()->getPhysicsTime(),
			(irr::u32)WorldManager::Get()->getRenderTime(),
			(irr::u32)WorldManager::Get()->getScriptTime(),
			(irr::u32)WorldManager::Get()->getSoundTime(),
			(irr::u32)WorldManager::Get()->getTransformTime(),
			(irr::u32)WorldManager::Get()->getGameplayTime());*/

		ImGui::End();
	}

	GameManager::Get()->updateUI(dt);
}

void EditorGameState::destroy()
{
	GameManager::Get()->destroy();
}

void EditorGameState::pause()
{
	GameManager::Get()->destroy();
}

void EditorGameState::resume()
{
	ImGui::GetIO().MouseDrawCursor = false;
	InputManager::Get()->centerMouse();

	GameManager::Get()->init(_asset_scn(std::string("editor/") + g_currentScene));
}
