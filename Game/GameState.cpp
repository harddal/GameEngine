#include "Game/GameState.h"

#include "Engine/Engine.h"

void GameState::init(std::string args)
{
	GameManager::Get()->init(args);
}

void GameState::update(float dt)
{
	GameManager::Get()->update(dt);
}

void GameState::updateUI(float dt)
{
	GameManager::Get()->updateUI(dt);
}

void GameState::destroy()
{
	GameManager::Get()->destroy();
}

void GameState::pause()
{
	GameManager::Get()->pause();
}

void GameState::resume()
{
	GameManager::Get()->resume();
}
