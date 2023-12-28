#pragma once

#include "Engine/EngineState.h"

class MenuState : public EngineState
{
public:
	explicit MenuState(ENGINE_STATE_ID id) : EngineState(id), m_isIngameMenu(false), m_escKeyLock(true) {}

	void init(std::string args = "") override;
	void update(float dt) override;
	void updateUI(float dt) override;
	void destroy() override;

	void pause() override;
	void resume() override;

private:
	bool
		m_isIngameMenu,
		m_escKeyLock; // Prevents menu from flickering when switching game to menu states
	
};
