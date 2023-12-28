#include "MainMenuGUI.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include "Utility/Utility.h"

#include "Engine/Input/InputManager.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;
using namespace boost;
using namespace filesystem;

using namespace ImGui;

static bool lockout_escape_key = false;

static bool
	show_window_background = true,
	show_window_load_game  = false,
	show_window_controls   = false,
	show_window_remap_key  = false,
	show_window_settings   = false,
	show_window_save_game  = false,
	show_window_name_save  = false;

bool MainMenu::IsEscapeKeyLocked()
{
	return lockout_escape_key;
}

void MainMenu::Reset()
{
	show_window_background = true;
	show_window_load_game  = false;
	show_window_controls   = false;
	show_window_remap_key  = false;
	show_window_settings   = false;
	show_window_save_game  = false;
	show_window_name_save  = false;
}

void MainMenu::Draw(bool is_ingame_menu)
{
	static bool window_open = true;

	static bool search_save_games = false;

	static int current_save_slot = 0;

	static vector<std::wstring> files;
	static vector<std::pair<bool, std::string>> saves;

	static sKeyActionPair currentKeyPair("null", KEY_UNKNOWN);
	
	PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
	SetNextWindowPos(ImVec2(0, 0));
	SetNextWindowSize(ImVec2(RenderManager::Get()->getConfiguration().width, RenderManager::Get()->getConfiguration().height));
	Begin("Background Window", &show_window_background,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
	End();
	PopStyleColor();
	
	if (!show_window_load_game && !show_window_save_game && !show_window_name_save && !show_window_settings && !show_window_controls && !show_window_remap_key)
	{
		Begin("MainMenu", &window_open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));

			if (!is_ingame_menu)
			{
				if (Button("New Game", _menu_button_size)) {
					show_window_save_game = false;
					show_window_controls = false;
					show_window_settings = false;
					show_window_load_game = false;

					Engine::Get()->stateManager()->setState(ESID_GAME, _asset_scn(GameManager::Get()->getConfiguration().new_game_scene));
				}
			}
			else
			{
				if (Button("Resume Game", _menu_button_size))
				{
					show_window_save_game = false;
					show_window_controls = false;
					show_window_settings = false;
					show_window_load_game = false;

					Engine::Get()->stateManager()->setStatePauseResume(ESID_GAME);
				}

				if (Button("Save Game", _menu_button_size))
				{
					search_save_games = true;
					
					show_window_save_game = true;

					show_window_controls = false;
					show_window_settings = false;
					show_window_load_game = false;
				}
			}


			if (Button("Load Game", _menu_button_size))
			{
				search_save_games = true;
				
				show_window_load_game = true;

				show_window_controls = false;
				show_window_settings = false;
				show_window_save_game = false;
			}

			if (Button("Controls", _menu_button_size))
			{
				show_window_controls = true;

				show_window_load_game = false;
				show_window_settings = false;
				show_window_save_game = false;
			}

			if (Button("Settings", _menu_button_size))
			{
				show_window_settings = true;

				show_window_load_game = false;
				show_window_controls = false;
				show_window_save_game = false;
			}

			if (Button("Exit", _menu_button_size))
			{
				Engine::Get()->exit();
			}

			End();
		}
	}

	if (search_save_games)
	{
		for (auto i = 0; i < 5; i++)
		{
			saves.emplace_back(std::pair<bool, std::string>(false, std::string()));
		}
		
		const path dir = "saves/";
		recursive_directory_iterator it(dir), end;

		for (auto& entry : make_iterator_range(it, end)) {
			if (is_regular(entry)) {
				files.emplace_back(entry.path().native());
			}
		}

		for (auto& file : files)
		{
			const auto
				filepath = string(file.begin(), file.end()),
				filename = Utility::FilenameFromPath(filepath),
				file_ext = Utility::FileExtensionFromPath(filepath);

			if (file_ext == string(".scn"))
			{
				if (filename.substr(0, 6) == "save00")
				{
					saves.at(0).first = true;
					if (filename.length() < 7)
					{
						saves.at(0).second = "Save 1";
					}
					else
					{
						saves.at(0).second = filename;
					}
				}

				if (filename.substr(0, 6) == "save01")
				{
					saves.at(1).first = true;
					if (filename.length() < 7)
					{
						saves.at(1).second = "Save 2";
					}
					else
					{
						saves.at(1).second = filename;
					}
				}

				if (filename.substr(0, 6) == "save02")
				{
					saves.at(2).first = true;
					if (filename.length() < 7)
					{
						saves.at(2).second = "Save 3";
					}
					else
					{
						saves.at(2).second = filename;
					}
				}

				if (filename.substr(0, 6) == "save03")
				{
					saves.at(3).first = true;
					if (filename.length() < 7)
					{
						saves.at(3).second = "Save 4";
					}
					else
					{
						saves.at(3).second = filename;
					}
				}

				if (filename.substr(0, 6) == "save04")
				{
					saves.at(4).first = true;
					if (filename.length() < 7)
					{
						saves.at(4).second = "Save 5";
					}
					else
					{
						saves.at(4).second = filename;
					}
				}
			}
		}

		search_save_games = false;
	}
	
	if (show_window_save_game) 
	{
		Begin("Save Game", &show_window_save_game, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));


			ImGui::PushID(std::string("BUTTON_SAVE_" + std::to_string(0)).c_str());
			if (Button(saves.at(0).first ? saves.at(0).second.substr(6).c_str() : " - Empty - ", _menu_button_size))
			{
				show_window_name_save = true;

				show_window_save_game = false;

				current_save_slot = 0;
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_SAVE_" + std::to_string(1)).c_str());
			if (Button(saves.at(1).first ? saves.at(1).second.substr(6).c_str() : " - Empty - ", _menu_button_size))
			{
				show_window_name_save = true;

				show_window_save_game = false;

				current_save_slot = 1;
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_SAVE_" + std::to_string(2)).c_str());
			if (Button(saves.at(2).first ? saves.at(2).second.substr(6).c_str() : " - Empty - ", _menu_button_size))
			{
				show_window_name_save = true;

				show_window_save_game = false;

				current_save_slot = 2;
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_SAVE_" + std::to_string(3)).c_str());
			if (Button(saves.at(3).first ? saves.at(3).second.substr(6).c_str() : " - Empty - ", _menu_button_size))
			{
				show_window_name_save = true;

				show_window_save_game = false;

				current_save_slot = 3;
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_SAVE_" + std::to_string(4)).c_str());
			if (Button(saves.at(4).first ? saves.at(4).second.substr(6).c_str() : " - Empty - ", _menu_button_size))
			{
				show_window_name_save = true;

				show_window_save_game = false;

				current_save_slot = 4;
			}
			ImGui::PopID();

			if (Button("Cancel", _menu_button_size))
			{
				show_window_save_game = false;
			}

			End();
		}
	}

	if (show_window_name_save)
	{
		Begin("Name Save Game", &show_window_name_save, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			lockout_escape_key = true;
			
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));
			
			static char buffer[20];

			static bool clear = true;
			if (clear)
			{
				memset(buffer, '\0', sizeof buffer);

				buffer[0] = 'S';
				buffer[1] = 'a';
				buffer[2] = 'v';
				buffer[3] = 'e';
				buffer[4] = ' ';
				buffer[5] = to_string(current_save_slot + 1).at(0);
				
				clear = false;
			}

			ImGui::Text("Name: ");
			
			ImGui::SameLine();
			
			ImGui::PushID("SaveNameTextBox");
			ImGui::InputText("", buffer, sizeof buffer);
			ImGui::PopID();

			ImGui::SameLine();

			if (Button("Clear", _window_button_size))
			{
				memset(buffer, '\0', sizeof buffer);
			}
			
			ImGui::SameLine();

			if (Button("Save", _window_button_size))
			{
				show_window_name_save = false;
				show_window_save_game = true;

				search_save_games = true;

				std::string name = buffer;

				WorldManager::Get()->exportScene(string("saves/save0" + to_string(current_save_slot) + name + ".scn"));

				clear = true;

				lockout_escape_key = false;
			}

			ImGui::SameLine();
			
			if (Button("Cancel", _window_button_size))
			{
				show_window_name_save = false;
				show_window_save_game = true;

				clear = true;

				lockout_escape_key = false;
			}
			
			End();
		}
	}
	
	if (show_window_load_game) 
	{
		Begin("Load Game", &show_window_load_game, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));

			ImGui::PushID(std::string("BUTTON_LOAD_" + std::to_string(0)).c_str());
			if (saves.at(0).first)
			{
				if (Button(saves.at(0).second.substr(6).c_str(), _menu_button_size))
				{
					if (is_ingame_menu)
					{
						Engine::Get()->stateManager()->destroyState(ESID_GAME);
					}

					Engine::Get()->stateManager()->setState(ESID_GAME, "saves/" + saves.at(0).second + ".scn");

					show_window_load_game = false;
				}
			}
			else
			{
				if (Button(" - Empty - ", _menu_button_size)) {}
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_LOAD_" + std::to_string(1)).c_str());
			if (saves.at(1).first)
			{
				if (Button(saves.at(1).second.substr(6).c_str(), _menu_button_size))
				{
					if (is_ingame_menu)
					{
						Engine::Get()->stateManager()->destroyState(ESID_GAME);
					}

					Engine::Get()->stateManager()->setState(ESID_GAME, "saves/" + saves.at(1).second + ".scn");

					show_window_load_game = false;
				}
			}
			else
			{
				if (Button(" - Empty - ", _menu_button_size)) {}
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_LOAD_" + std::to_string(2)).c_str());
			if (saves.at(2).first)
			{
				if (Button(saves.at(2).second.substr(6).c_str(), _menu_button_size))
				{
					if (is_ingame_menu)
					{
						Engine::Get()->stateManager()->destroyState(ESID_GAME);
					}

					Engine::Get()->stateManager()->setState(ESID_GAME, "saves/" + saves.at(2).second + ".scn");

					show_window_load_game = false;
				}
			}
			else
			{
				if (Button(" - Empty - ", _menu_button_size)) {}
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_LOAD_" + std::to_string(3)).c_str());
			if (saves.at(3).first)
			{
				if (Button(saves.at(3).second.substr(6).c_str(), _menu_button_size))
				{
					if (is_ingame_menu)
					{
						Engine::Get()->stateManager()->destroyState(ESID_GAME);
					}

					Engine::Get()->stateManager()->setState(ESID_GAME, "saves/" + saves.at(3).second + ".scn");

					show_window_load_game = false;
				}
			}
			else
			{
				if (Button(" - Empty - ", _menu_button_size)) {}
			}
			ImGui::PopID();

			ImGui::PushID(std::string("BUTTON_LOAD_" + std::to_string(4)).c_str());
			if (saves.at(4).first)
			{
				if (Button(saves.at(4).second.substr(6).c_str(), _menu_button_size))
				{
					if (is_ingame_menu)
					{
						Engine::Get()->stateManager()->destroyState(ESID_GAME);
					}

					Engine::Get()->stateManager()->setState(ESID_GAME, "saves/" + saves.at(4).second + ".scn");

					show_window_load_game = false;
				}
			}
			else
			{
				if (Button(" - Empty - ", _menu_button_size)) {}
			}
			ImGui::PopID();
			
			if (Button("Cancel", _menu_button_size))
			{
				show_window_load_game = false;
			}
			
			End();
		}
	}

	if (show_window_settings)
	{
		Begin("Settings", &show_window_settings, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			lockout_escape_key = true;
			
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));

			static auto render = RenderManager::Get()->getConfiguration();
			static auto physics = PhysicsManager::Get()->getConfiguration();
			static auto sound = SoundManager::Get()->getConfiguration();

			ImGui::Text("- Display -");
			ImGui::Spacing();

			static bool invalidMode = false;

			if (render.aspect == -1 || render.mode == -1)
			{
				invalidMode = true;
			}

			ImGui::Checkbox("Custom Resolution", &invalidMode);
			
			static int currentaspectRatioItem = render.aspect;
			static int currentResolutionItem = render.mode;

			if (!invalidMode)
			{
				const char* aspectRatioItems[] = { "4:3", "5:4", "16:9", "16:10" };
				Combo("Aspect Ratio", &currentaspectRatioItem, aspectRatioItems, static_cast<int>((sizeof aspectRatioItems / sizeof *aspectRatioItems)));

				switch (currentaspectRatioItem)
				{
				case 0:
				{
					const char* resolutionItems[] = { "640x480", "800x600", "1024x768", "1280x960", "1600x1200", "2048x1536" };

					if (currentResolutionItem > 5)
					{
						currentResolutionItem = 0;
					}
					else if (currentResolutionItem == -1)
					{
						invalidMode = true;
						break;
					}

					Combo("Resolution", &currentResolutionItem, resolutionItems, static_cast<int>((sizeof resolutionItems / sizeof *resolutionItems)));

					break;
				}

				case 1:
				{
					const char* resolutionItems[] = { "1280x1024", "2560x2048" };

					if (currentResolutionItem > 1)
					{
						currentResolutionItem = 0;
					}
					else if (currentResolutionItem == -1)
					{
						invalidMode = true;
						break;
					}

					Combo("Resolution", &currentResolutionItem, resolutionItems, static_cast<int>((sizeof resolutionItems / sizeof *resolutionItems)));
	
					break;
				}

				case 2:
				{
					const char* resolutionItems[] = { "1280x720", "1366x768", "1600x900", "1920x1080", "2550x1440", "3840x2160" };

					if (currentResolutionItem > 5)
					{
						currentResolutionItem = 0;
					}
					else if (currentResolutionItem == -1)
					{
						invalidMode = true;
						break;
					}

					Combo("Resolution", &currentResolutionItem, resolutionItems, static_cast<int>((sizeof resolutionItems / sizeof *resolutionItems)));
	
					break;
				}

				case 3:
				{
					const char* resolutionItems[] = { "1280x800", "1440x900", "1680x1050", "1920x1200", "2560x1600", "3840x2400" };

					if (currentResolutionItem > 5)
					{
						currentResolutionItem = 0;
					}
					else if (currentResolutionItem == -1)
					{
						invalidMode = true;
						break;
					}

					Combo("Resolution", &currentResolutionItem, resolutionItems, static_cast<int>((sizeof resolutionItems / sizeof *resolutionItems)));

					break;
				}

				default: invalidMode = true;
					break;
				}
			}
			
			if (invalidMode)
			{
				InputInt("Width", &render.width);
				InputInt("Height", &render.height);
			}

			ImGui::Checkbox("Fullscreen", &render.fullscreen);
			ImGui::Checkbox("Vertical Sync", &render.vSync);
			ImGui::SliderInt("Framerate Limit", &render.frameLimit, 30, 240);
			ImGui::SliderInt("Aniostropic Filtering", &render.anisotropyFactor, 0, 16);
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("- Sound -");
			ImGui::Spacing();
			ImGui::SliderFloat("Master Volume", &sound.volume, 0.f, 1.f);
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("- Physics -");
			ImGui::Spacing();
			ImGui::Checkbox("GPU Acceleration", &physics.gpu);
			ImGui::SliderInt("CPU Threads", &physics.threads, 1, static_cast<int>(std::thread::hardware_concurrency()));
			
			ImGui::Spacing();
			ImGui::Spacing();

			if (Button("Save", _window_button_size))
			{
				if (!invalidMode)
				{
					render.aspect = currentaspectRatioItem;
					render.mode = currentResolutionItem;

					RenderManager::Get()->getResolutionFromMode(render.aspect, render.mode, render.width, render.height);
				}
				else
				{
					render.aspect = -1;
					render.mode = -1;
				}
				
				RenderManager::Get()->saveConfiguration(render);
				PhysicsManager::Get()->saveConfiguration(physics);
				SoundManager::Get()->saveConfiguration(sound);

				render = RenderManager::Get()->getConfiguration();
				physics = PhysicsManager::Get()->getConfiguration();
				sound = SoundManager::Get()->getConfiguration();

				show_window_settings = false;

				lockout_escape_key = false;
			}
			
			ImGui::SameLine();
			
			if (Button("Cancel", _window_button_size))
			{			
				render = RenderManager::Get()->getConfiguration();
				physics = PhysicsManager::Get()->getConfiguration();
				sound = SoundManager::Get()->getConfiguration();

				currentaspectRatioItem = render.aspect;
				currentResolutionItem = render.mode;
				
				show_window_settings = false;

				lockout_escape_key = false;
			}

			End();
		}
	}

	if (show_window_controls)
	{
		Begin("Controls", &show_window_controls, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			lockout_escape_key = true;
			
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));

			static auto config = InputManager::Get()->getConfiguration();

			config.key_forward = InputManager::Get()->getKeyActionPairList().at(0).key;
			config.key_backward = InputManager::Get()->getKeyActionPairList().at(1).key;
			config.key_strafel = InputManager::Get()->getKeyActionPairList().at(2).key;
			config.key_strafer = InputManager::Get()->getKeyActionPairList().at(3).key;
			config.key_jump = InputManager::Get()->getKeyActionPairList().at(4).key;
			config.key_crouch = InputManager::Get()->getKeyActionPairList().at(5).key;
			config.key_sprint = InputManager::Get()->getKeyActionPairList().at(6).key;
			config.key_use = InputManager::Get()->getKeyActionPairList().at(7).key;
			config.key_reload = InputManager::Get()->getKeyActionPairList().at(8).key;
			config.key_inventory = InputManager::Get()->getKeyActionPairList().at(9).key;
			
			ImGui::Text("Forward:      "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_forward + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("forward", config.key_forward);
			}
			
			ImGui::Text("Backward:     "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_backward + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("backward", config.key_backward);
			}
			
			ImGui::Text("Strafe Left:  "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_strafel + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("strafel", config.key_strafel);
			}
			
			ImGui::Text("Strafe Right: "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_strafer + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("strafer", config.key_strafer);
			}
			
			ImGui::Text("Jump:         "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_jump + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("jump", config.key_jump);
			}
			
			ImGui::Text("Crouch:       "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_crouch + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("crouch", config.key_crouch);
			}
			
			ImGui::Text("Sprint:       "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_sprint + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("sprint", config.key_sprint);
			}
			
			ImGui::Text("Interact:     "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_use + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("use", config.key_use);
			}
			
			ImGui::Text("Reload:       "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_reload + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("reload", config.key_reload);
			}
			
			ImGui::Text("Inventory:    "); ImGui::SameLine(); if (Button(KEYBOARD_KEY_STRING[config.key_inventory + 1], _window_button_size))
			{
				show_window_controls = false;
				show_window_remap_key = true;

				currentKeyPair = sKeyActionPair("inventory", config.key_inventory);
			}

			ImGui::Spacing();
			ImGui::Spacing();
			
			ImGui::SliderFloat("Mouse Sensitivity", &config.xsens, 0.05f, 1.f);

			ImGui::Spacing();
			ImGui::Spacing();
			
			if (Button("Save", _window_button_size))
			{
				config.ysens = config.xsens;

				InputManager::Get()->saveConfiguration(config);

				config = InputManager::Get()->getConfiguration();
				
				show_window_controls = false;

				lockout_escape_key = false;
			}
			ImGui::SameLine();
			
			if (Button("Cancel", _window_button_size))
			{
				config = InputManager::Get()->getConfiguration();

				InputManager::Get()->getKeyActionPairList().clear();

				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("forward", config.key_forward));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("backward", config.key_backward));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("strafel", config.key_strafel));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("strafer", config.key_strafer));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("jump", config.key_jump));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("crouch", config.key_crouch));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("sprint", config.key_sprint));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("use", config.key_use));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("reload", config.key_reload));
				InputManager::Get()->getKeyActionPairList().emplace_back(sKeyActionPair("inventory", config.key_inventory));
				
				show_window_controls = false;

				lockout_escape_key = false;
			}
			
			End();
		}
	}

	if (show_window_remap_key)
	{
		Begin("Remap Input", &show_window_remap_key, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		{
			lockout_escape_key = true;
			
			SetWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width / 2 - GetWindowSize().x / 2, RenderManager::Get()->getConfiguration().height / 2 - GetWindowSize().y / 2));

			ImGui::Text(string("Press a key to rebind \'" + currentKeyPair.getAction() + "\'").c_str());

			for (auto i = 0; i < KEY_F1; i++)
			{
				if (InputManager::Get()->isKeyPressed(i, true))
				{
					if (i == KEY_ESCAPE) 
					{
						show_window_remap_key = false;
						show_window_controls = true;

						currentKeyPair = sKeyActionPair("null", KEY_UNKNOWN);

						lockout_escape_key = false;
						
						break;
					}
					
					if (i != KEY_TILDE && i != KEY_LSYSTEM && i != KEY_RSYSTEM && i != KEY_MENU && i != KEY_BACKSPACE)
					{
						for (auto& pair : InputManager::Get()->getKeyActionPairList())
						{
							if (pair.getAction() == currentKeyPair.getAction())
							{
								pair.key = i;

								show_window_remap_key = false;
								show_window_controls = true;

								currentKeyPair = sKeyActionPair("null", KEY_UNKNOWN);

								lockout_escape_key = false;

								break;
							}
						}

						break;
					}
				}
			}
			
			End();
		}
	}
}
