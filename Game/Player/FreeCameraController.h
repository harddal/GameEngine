#pragma once

#include "anax/anax.hpp"

class FreeCameraController
{
public:
	FreeCameraController();
	~FreeCameraController() = default;

	void init();
	void update(float dt);
	void destroy();

	void pause();
	void resume();

	bool isMoving() { return m_isMoving; }

private:
	bool m_isMoving = false, m_firstUpdate = true, m_used = false;

};

extern std::unique_ptr<FreeCameraController> g_FreeCameraController;
