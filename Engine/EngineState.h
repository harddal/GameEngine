#pragma once

#include <string>

enum ENGINE_STATE_ID
{
    ESID_DEBUG,
    ESID_INTRO,
    ESID_MENU,
    ESID_GAME,
    ESID_ENDGAME,
    ESID_EDITOR,
    ESID_EDITORGAME,
    ESID_INVALID
};

class EngineState
{
public:
    explicit EngineState(ENGINE_STATE_ID id) : m_id(id), m_hasInitialized(false), m_isPaused(false) {}

    virtual void init(std::string args = "") = 0;
    virtual void update(float dt) = 0;
    virtual void updateUI(float dt) = 0;
    virtual void destroy() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    bool hasInitialized() const { return m_hasInitialized; }
    void setInitialized(bool init = true) { m_hasInitialized = init; }
	bool isPaused() const { return m_isPaused; }
	void setPaused(bool paused = true) { m_isPaused = paused; }

    ENGINE_STATE_ID getID() const { return m_id; }
    void setID(ENGINE_STATE_ID id) { m_id = id; }

protected:
    ENGINE_STATE_ID m_id;
    bool m_hasInitialized, m_isPaused;
};
