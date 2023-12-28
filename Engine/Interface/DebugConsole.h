#pragma once

#include <string>
#include <vector>

#include "Engine/Types.h"

class DebugConsole
{
public:
    DebugConsole() : m_scrollToBottom(true), m_drawStats(false), m_drawPlayer(false), m_drawProps(false)
    {
        m_selectedEntity = _entity_null_value;

        clearInputBuffer();
    }

    void draw(double dt);
    void draw_stats();
    void drawPlayerInfo();

    void log(const std::string& str);
    void logwarn(const std::string& str);
    void logerr(const std::string& str);
    void logerr_para(const std::string& str);


    void clearInputBuffer();

    void toggleDrawStats()
    {
        m_drawStats = !m_drawStats;
        if (m_drawStats) { m_drawPlayer = false; }
    }

    bool drawStats() { return m_drawStats; }

    void togglePlayerInfo()
    {
        m_drawPlayer = !m_drawPlayer;
        if (m_drawPlayer) { m_drawStats = false; }
    }

    bool playerInfo() { return m_drawPlayer; }

    void executecmd(const std::string& cmd);

    std::vector<std::string>* getStringDisplayList() { return &m_stringDisplayList; }

private:

    void toggleEntityProps() { m_drawProps = !m_drawProps; }
    bool entityProps() { return m_drawProps; }
	void toggleHiearch() { m_drawHiearch = !m_drawHiearch; }
	bool hiearch() { return m_drawHiearch; }

    int argtobool(const std::string& arg);

    bool m_scrollToBottom, m_drawStats, m_drawPlayer, m_drawProps, m_drawHiearch;

    unsigned int m_selectedEntity;

    char m_inputBuffer[256];
    std::vector<std::string> m_stringDisplayList;
};
