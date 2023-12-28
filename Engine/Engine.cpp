#include "Engine/Engine.h"

#include <IMGUI/imgui.h>

#include "Debug/DebugState.h"
#include "Editor/EditorState.h"
#include "Editor/EditorGameState.h"
#include "Game/IntroState.h"
#include "Game/MenuState.h"
#include "Game/GameState.h"
#include "Game/EndGameState.h"
#include "Resource/FilePaths.h"

#include "Utility/Utility.h"

#include <windows.h>

Engine* Engine::s_Instance = nullptr;

Engine::Engine(const std::string& name, const std::string& args) :
	m_exit(false), m_isGameMode(false), m_isEditorMode(false), m_drawConsole(false),
	m_EnableDefaultStatsDrawing(false), m_EnableDebugConsole(false), m_EnableGameDebugFeatures(false),
	m_currentTick(0), m_lastTick(0), m_deltaTime(0.0f), m_renderManager(name, args)
{
    m_cmdLineArgs = args;

	setEditorMode(Utility::GetCmdlOptionExists(m_cmdLineArgs, "editor"));

	if (!isEditorMode())
	{
		if (Utility::GetCmdlOptionExists(m_cmdLineArgs, "console"))
		{
			m_EnableDebugConsole = true;
		}
		if (Utility::GetCmdlOptionExists(m_cmdLineArgs, "debug"))
		{
			m_EnableDefaultStatsDrawing = true;
			m_EnableDebugConsole = true;
			m_EnableGameDebugFeatures = true;
		}
	}

    if (s_Instance)
    {
        Utility::Error("Pointer to class \'Engine\' is invalid");
    }
    s_Instance = this;
	
    m_materialBuilder.buildMaterialTable();

	StartCounter();
}
Engine::~Engine()
{
    // BUG: Causes stack overflow on exit
    //delete s_Instance;
}

void Engine::init()
{
#ifndef NDEBUG
	setGameDebugFeaturesEnabled(true);
#endif

	if (isEditorMode())
	{
		m_stateManager.setState(ESID_EDITOR);
	}
	else
	{
		// Bypass intro and menu for debug testing
		if (Utility::GetCmdlOptionExists(m_cmdLineArgs, "debug"))
		{
			m_stateManager.setState(ESID_GAME, _asset_scn(m_gameManager.getConfiguration().new_game_scene));
		}
		// Bypass intro
		else if (Utility::GetCmdlOptionExists(m_cmdLineArgs, "nointro"))
		{
			m_stateManager.setState(ESID_MENU);
		}
		// Normal startup
		else
		{
			m_stateManager.setState(ESID_INTRO);
		}
	}
}

void Engine::update()
{
    while (m_renderManager.device()->run() && !m_exit)
    {
		m_currentTick = static_cast<float>(GetCounter());//static_cast<irr::f32>(RenderManager::Get()->device()->getTimer()->getTime());

        if (m_currentTick - m_lastTick > 16.67f) 
        {
            m_deltaTime = m_currentTick - m_lastTick;

        	// Initialization queue for new states
			if (m_stateManager.isNewState())
			{
				if (m_stateManager.next() != ESID_INVALID)
				{
					m_stateManager.initState(m_stateManager.next(), m_stateManager.getStateInitArgs());
					m_stateManager.cycleNextState();
				}
			}

            m_renderManager.beginImGui();
            {
                if (m_isGameMode)
                {
					if (m_EnableDebugConsole)
					{
						static auto tilde_released = false;
						if (m_inputManager.getKeyRelease(KEY_TILDE, &tilde_released, true)) 
						{
							m_drawConsole = !m_drawConsole;

							InputManager::Get()->centerMouse();
							
							m_debugConsole.clearInputBuffer();
						}
					}

					static bool resetInputProcessingFlag = false;
                	
                    if (m_drawConsole)
                    {
                        m_inputManager.canProcessInput(false);

						m_debugConsole.draw(m_deltaTime);

						resetInputProcessingFlag = true;
                    }
					else
					{
						if (resetInputProcessingFlag)
						{
							m_inputManager.canProcessInput(true);

							m_debugConsole.draw_stats();

							resetInputProcessingFlag = false;
						}
					}
                }
            }

			// DEBUG Find out best update order

            m_inputManager.update(true);

            m_worldManager.update(m_deltaTime);

            m_stateManager.update(m_deltaTime);
            m_stateManager.updateUI(m_deltaTime);

			if (m_isGameMode)
			{
				m_currentPhysicsTick = static_cast<float>(GetCounter());
				m_physicsManager.update(m_deltaTime);
				m_physicsTime = static_cast<float>(GetCounter()) - m_currentPhysicsTick;
			}

			if (m_renderManager.Get()->getConfiguration().frameLimit > 0)
			{
				m_currentRenderTick = static_cast<float>(GetCounter());
				m_renderManager.draw(m_deltaTime);
				m_renderTime = static_cast<float>(GetCounter()) - m_currentRenderTick;
			}

            m_lastTick = m_currentTick;
        }

		if (m_renderManager.Get()->getConfiguration().frameLimit == 0)
		{
			m_renderManager.draw(m_deltaTime);
		}
    }
}

Engine::StateManager::StateManager() : m_current(ESID_INVALID), m_next(ESID_INVALID), m_previous(ESID_INVALID), m_initNewState(false)
{
    m_states.push_back(std::make_unique<DebugState>(ESID_DEBUG));
    m_states.push_back(std::make_unique<IntroState>(ESID_INTRO));
    m_states.push_back(std::make_unique<MenuState>(ESID_MENU));
    m_states.push_back(std::make_unique<GameState>(ESID_GAME));
    m_states.push_back(std::make_unique<EndGameState>(ESID_ENDGAME));
    m_states.push_back(std::make_unique<EditorState>(ESID_EDITOR));
    m_states.push_back(std::make_unique<EditorGameState>(ESID_EDITORGAME));
}

Engine::StateManager::~StateManager()
{
    for (auto i = 0U; i < m_states.size(); i++) 
	{
        m_states[i]->destroy();
    }

    m_states.clear();
    std::vector<std::unique_ptr<EngineState>>().swap(m_states);
}

void Engine::StateManager::update(irr::f32 dt)
{
    if (m_current < ESID_INVALID) 
	{
        if (m_states[m_current]->hasInitialized()) 
		{
            m_states[m_current]->update(dt);
        }
    }
}

void Engine::StateManager::updateUI(irr::f32 dt)
{
    if (m_current < ESID_INVALID) 
	{
        if (m_states[m_current]->hasInitialized()) 
		{
            m_states[m_current]->updateUI(dt);
        }
    }
}

ENGINE_STATE_ID Engine::StateManager::setState(ENGINE_STATE_ID state, const std::string& args)
{
    if (state != ESID_INVALID) 
	{
        if (!m_states[state]->hasInitialized()) 
		{
			m_initNewState = true;
        }

		m_next = static_cast<unsigned int>(state);

		m_args = args;
    }

    return state;
}

ENGINE_STATE_ID Engine::StateManager::setStatePauseResume(ENGINE_STATE_ID state, const std::string& args)
{
    if (state != ESID_INVALID) 
	{
		if (!m_states[m_current]->hasInitialized())
		{
			spdlog::error("Current active state \'{0}\' is not initialized in StateManager::setStatePauseResume()", m_current);
			return ESID_INVALID;
		}
		if (!m_states[state]->hasInitialized())
		{
			spdlog::error("Next state \'{0}\' is not initialized in StateManager::setStatePauseResume()", static_cast<irr::u32>(state));
			return ESID_INVALID;
		}
    	
        m_states[m_current]->pause();
		m_states[m_current]->setPaused();

		m_states[state]->setPaused(false);
        m_states[state]->resume();

		m_current = static_cast<unsigned int>(state);

		m_args = args;
    }
	
    return state;
}

ENGINE_STATE_ID Engine::StateManager::initState(ENGINE_STATE_ID state, const std::string& args)
{
    if (state != ESID_INVALID) 
	{
        if (!m_states[state]->hasInitialized()) 
		{
            m_states[state]->init(args);
            m_states[state]->setInitialized();
        }
    }
    return state;
}

ENGINE_STATE_ID Engine::StateManager::destroyState(ENGINE_STATE_ID state)
{
    if (state != ESID_INVALID && m_states[state]->hasInitialized()) 
	{
    	if (m_states[state]->getID() == m_current)
    	{
			spdlog::error("StateManager: Cannot destroy currently active state");
			return state;
    	}
    	
        m_states[state]->setInitialized(false);
        m_states[state]->destroy();

		if (m_next == state)
		{
			m_next = ESID_INVALID;
		}
    	if (m_previous == state)
    	{
			m_previous = ESID_INVALID;
    	}
    }
    return state;
}

ENGINE_STATE_ID Engine::StateManager::pauseState(ENGINE_STATE_ID state)
{
    if (state != ESID_INVALID) 
	{
        m_states[state]->pause();
		m_states[state]->setPaused();
    }
    return state;
}

ENGINE_STATE_ID Engine::StateManager::resumeState(ENGINE_STATE_ID state)
{
    if (state != ESID_INVALID) 
	{
        m_states[state]->resume();
		m_states[state]->setPaused(false);
    }
    return state;
}

void Engine::StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		std::cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double Engine::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}