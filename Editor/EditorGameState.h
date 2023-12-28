#pragma once

#include "Engine/EngineState.h"

#include "Game/GameplaySystem.h"
#include "Game/Player/PlayerController.h"

class EditorGameState : public EngineState
{
public:
	explicit EditorGameState(ENGINE_STATE_ID id) : EngineState(id) {}

	void init(std::string args = "") override;
	void update(float dt) override;
	void updateUI(float dt) override;
	void destroy() override;

	void pause() override;
	void resume() override;

};
