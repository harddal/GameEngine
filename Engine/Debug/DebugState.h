#pragma once

#include "Engine/EngineState.h"

class DebugState : public EngineState
{
public:
	explicit DebugState(ENGINE_STATE_ID id) : EngineState(id) {}

	void init(std::string args = "") override;
	void update(float dt) override;
	void updateUI(float dt) override;
	void destroy() override;

	void pause() override;
	void resume() override;
};
