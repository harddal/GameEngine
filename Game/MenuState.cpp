#include "Game/MenuState.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"

#include "Interface/MainMenuGUI.h"

void MenuState::init(std::string args)
{
	ImGui::GetIO().MouseDrawCursor = true;
}

void MenuState::update(float dt)
{
	ImGui::GetIO().MouseDrawCursor = true;

	if (m_isIngameMenu)
	{
		if (InputManager::Get()->getKeyPressOnce(KEY_ESCAPE, &m_escKeyLock, true) && !MainMenu::IsEscapeKeyLocked())
		{
			Engine::Get()->stateManager()->setStatePauseResume(ESID_GAME);
		}
	}
}

void MenuState::updateUI(float dt)
{
	MainMenu::Draw(m_isIngameMenu);
}

void MenuState::destroy()
{

}

void MenuState::pause()
{

}

void MenuState::resume()
{
	MainMenu::Reset();
	
	m_isIngameMenu = true;

	InputManager::Get()->centerMouse();

	m_escKeyLock = true;
}
