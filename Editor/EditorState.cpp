#include "Editor/EditorState.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"
#include "Interface/EditorInterface.h"

SceneInteractionManager g_sceneInteractor;

void EditorState::init(std::string args)
{
	ImGui::GetIO().MouseDrawCursor = false;
	RenderManager::Get()->device()->getCursorControl()->setVisible(true);
	
    m_camera.init();

	InputManager::Get()->centerMouse();
	
	WorldManager::Get()->renderSystem()->setDebugSpriteVisible(true);

	PhysicsManager::Get()->createScene();
	
	g_sceneInteractor.init();

	Engine::Get()->stateManager()->initState(ESID_EDITORGAME);

	WorldManager::Get()->importScene(_asset_scn("npc_test_ground"));
}

void EditorState::update(float dt)
{
	WorldManager::Get()->updateEntityQueues();
	WorldManager::Get()->transformSystem()->update();
	WorldManager::Get()->renderSystem()->forceTransformUpdate();
	
    m_camera.update();

	g_sceneInteractor.update();
}

void EditorState::updateUI(float dt)
{
	EditorInterface::draw();
}

void EditorState::destroy()
{
    WorldManager::Get()->killAllEntities();
	WorldManager::Get()->clearCVars();
	
	m_camera.destroy();
	g_sceneInteractor.destroy();
	
	PhysicsManager::Get()->destroyScene();
}

void EditorState::pause()
{
	RenderManager::Get()->device()->getCursorControl()->setVisible(false);
	WorldManager::Get()->renderSystem()->setDebugSpriteVisible(false);

	SceneDescriptor scenedesc;
	scenedesc.ambient_light = RenderManager::Get()->sceneManager()->getAmbientLight();
	scenedesc.skydome_texture = RenderManager::Get()->getCurrentSkydomeTexture();
	scenedesc.name = g_currentScene;
	WorldManager::Get()->exportScene(_asset_scn(std::string("editor/") + g_currentScene));

	WorldManager::Get()->killAllEntities();
	WorldManager::Get()->clearCVars();
	
	PhysicsManager::Get()->destroyScene();
}

void EditorState::resume()
{
    InputManager::Get()->centerMouse();

	ImGui::GetIO().MouseDrawCursor = false;
	RenderManager::Get()->device()->getCursorControl()->setVisible(true);
	WorldManager::Get()->renderSystem()->setDebugSpriteVisible(true);

	PhysicsManager::Get()->createScene();
	
	WorldManager::Get()->importScene(_asset_scn(std::string("editor/") + g_currentScene));
	
	m_camera.reset();
}
