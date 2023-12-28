#pragma once

#include <irrTypes.h>

#include "Engine/EngineState.h"
#include "Engine/Resource/FilePaths.h"

#define _new_game_starting_scene _asset_scn("devtest")

#define _menu_button_size   ImVec2(150 * RenderManager::Get()->getConfiguration().dpi_scale, 35 * RenderManager::Get()->getConfiguration().dpi_scale)
#define _window_button_size ImVec2(70 * RenderManager::Get()->getConfiguration().dpi_scale, 20 * RenderManager::Get()->getConfiguration().dpi_scale)

class MainMenu
{
public:
	static bool IsEscapeKeyLocked();
	static void Reset();
	static void Draw(bool is_ingame_menu);
};
