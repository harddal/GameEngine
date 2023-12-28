#pragma once

#include "Game/GameManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Physics/PhysicsManager.h"
#include "Engine/Renderer/RenderManager.h"
#include "Engine/Script/ScriptManager.h"
#include "Engine/Sound/SoundManager.h"
#include "Engine/World/WorldManager.h"

#include "Engine/Resource/MaterialBuilder.h"

#include "EngineState.h"

#include "Engine/Interface/DebugConsole.h"

class Engine
{
protected:
    class StateManager
    {
    public:
        StateManager();
        ~StateManager();

        void update(irr::f32 dt);
        void updateUI(irr::f32 dt);

		std::string getStateInitArgs() { std::string temp = m_args; m_args = std::string(); return temp; }

		bool isNewState() { if (m_initNewState) { m_initNewState = false; return true; } return false; }
		
		void cycleNextState() { m_previous = m_current; m_current = m_next; m_next = ESID_INVALID; }

        ENGINE_STATE_ID current()  const { return static_cast<ENGINE_STATE_ID>(m_current);  }
		ENGINE_STATE_ID next()     const { return static_cast<ENGINE_STATE_ID>(m_next);     }
		ENGINE_STATE_ID previous() const { return static_cast<ENGINE_STATE_ID>(m_previous); }

        ENGINE_STATE_ID setState(ENGINE_STATE_ID state, const std::string& args = "");

    	// Both states must already be initialized
        ENGINE_STATE_ID setStatePauseResume(ENGINE_STATE_ID state, const std::string& args = "");

    	// Hard inits state, ignoring the queue
        ENGINE_STATE_ID initState(ENGINE_STATE_ID state, const std::string& args = "");
        ENGINE_STATE_ID destroyState(ENGINE_STATE_ID state);

        ENGINE_STATE_ID pauseState(ENGINE_STATE_ID state);
        ENGINE_STATE_ID resumeState(ENGINE_STATE_ID state);

        std::vector<std::unique_ptr<EngineState>> m_states;

        irr::u32 m_current, m_next, m_previous;

		bool m_initNewState;

		std::string m_args;
    };

public:
    Engine& operator =(const Engine&) = delete;

    Engine(const std::string& name = std::string(), const std::string& args = std::string());
    ~Engine();

	void init();
	
    void update();

	void exit() { m_exit = true; }

	std::string getCmdLineArgs() { return m_cmdLineArgs; }
	
    irr::u32 getCurrentTime() const { return m_currentTick; }
    irr::f32 getDeltaTime() const { return m_deltaTime; }

	void StartCounter();
	double GetCounter();

    bool isGameMode() const { return m_isGameMode; }
    void setGameMode(bool mode = true) { m_isGameMode = mode; }

	bool isEditorMode() const { return m_isEditorMode; }
	void setEditorMode(bool mode = true) { m_isEditorMode = mode; }

	bool isDebugConsoleVisible() const { return m_drawConsole; }

	bool isDefaultStatsDrawingEnabled() const { return m_EnableDefaultStatsDrawing; }
	bool isDebugConsoleEnabled() const { return m_EnableDebugConsole; }
	bool isGameDebugFeaturesEnabled() const { return m_EnableGameDebugFeatures; }
	void setDefaultStatsDrawingEnabled(bool enabled = true) { m_EnableDefaultStatsDrawing = enabled; }
	void setDebugConsoleEnabled(bool enabled = true) { m_EnableDebugConsole = enabled; }
	void setGameDebugFeaturesEnabled(bool enabled = true) { m_EnableGameDebugFeatures = enabled; }

    StateManager* stateManager() { return &m_stateManager; }

    MaterialBuilder getMaterialBuilder() { return m_materialBuilder; }

	irr::f32 getPhysicsTime() { return m_physicsTime; }
	irr::f32 getRenderTime() { return m_renderTime; }

    static Engine* Get() { return s_Instance; }

private:
    static Engine* s_Instance;
	
    MaterialBuilder m_materialBuilder;

    bool
		m_exit, m_isGameMode, m_isEditorMode, m_drawConsole,
		m_EnableDefaultStatsDrawing, m_EnableDebugConsole, m_EnableGameDebugFeatures;

	double PCFreq = 0.0;
	__int64 CounterStart = 0;

    irr::f32 m_currentTick, m_lastTick;
    irr::f32 m_deltaTime;

	irr::f32 m_currentPhysicsTick, m_physicsTime;
	irr::f32 m_currentRenderTick, m_renderTime;

    std::string m_cmdLineArgs;

    StateManager m_stateManager;

    DebugConsole m_debugConsole;

	GameManager m_gameManager;
    InputManager m_inputManager;
    PhysicsManager m_physicsManager;
    RenderManager m_renderManager;
    ScriptManager m_scriptManager;
    SoundManager m_soundManager;
    WorldManager m_worldManager;
};
