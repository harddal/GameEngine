#pragma once

#include <string>

#include "cereal/cereal.hpp"

struct GameConfiguration
{
	std::string new_game_scene;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(new_game_scene));
	}

	GameConfiguration() :
		new_game_scene("default")
	{}
};

class GameManager
{
public:
	GameManager& operator =(const GameManager&) = delete;

	GameManager();
	~GameManager() = default;

	void init(const std::string &args);
	void update(float dt, bool editor_mode = false);
	void updateUI(float dt);
	void destroy();
	void reset();
	void pause();
	void resume();

	GameConfiguration getConfiguration() const { return m_configuration; }
	void loadConfiguration();
	void saveConfiguration(GameConfiguration& configuration);

	static GameManager* Get() { return s_Instance; }

private:
	static GameManager* s_Instance;

	GameConfiguration m_configuration;

	bool m_hasGameInitialized;

	std::string m_currentGameArguments;
};
