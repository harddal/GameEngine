#pragma once

#include "Engine/EngineState.h"

#include "Editor/EditorCamera.h"
#include "Editor/SceneInteractionManager.h"

extern SceneInteractionManager g_sceneInteractor;

class EditorState : public EngineState
{
public:
	explicit EditorState(ENGINE_STATE_ID id) : EngineState(id) {}

	void init(std::string args = "") override;
	void update(float dt) override;
	void updateUI(float dt) override;
	void destroy() override;

	void pause() override;
	void resume() override;

private:
    EditorCamera m_camera;
	
};
