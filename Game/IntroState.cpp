#include "Game/IntroState.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"

void IntroState::init(std::string args)
{
	ImGui::GetIO().MouseDrawCursor = false;
}

void IntroState::update(float dt)
{
	Engine::Get()->stateManager()->setState(ESID_MENU);
}

void IntroState::updateUI(float dt)
{

}

void IntroState::destroy()
{
	
}

void IntroState::pause()
{
	
}

void IntroState::resume()
{
	
}
