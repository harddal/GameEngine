#include "Engine/Input/InputManager.h"

#include "Engine/Input/InputMap.h"
#include "Utility/Utility.h"

#include <fstream>

#include <Windows.h>
#include <spdlog/spdlog.h>

#include "Engine/Engine.h"

InputManager* InputManager::s_Instance = nullptr;

std::array<const char*, KEY_KEYCOUNT + 2> KEYBOARD_KEY_STRING =
{
	"NONE",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"ESCAPE",
	"LCONTROL",
	"LSHIFT",
	"LALT",
	"LSYSTEM",
	"RCONTROL",
	"RSHIFT",
	"RALT",
	"RSYSTEM",
	"MENU",
	"LBRACKET",
	"RBRACKET",
	"SEMICOLON",
	"COMMA",
	"PERIOD",
	"QUOTE",
	"SLASH",
	"BACKSLASH",
	"TILDE",
	"EQUAL",
	"DASH",
	"SPACE",
	"RETURN",
	"BACKSPACE",
	"TAB",
	"PAGEUP",
	"PAGEDOWN",
	"END",
	"HOME",
	"INSERT",
	"DELETE",
	"ADD",
	"SUBTRACT",
	"MULTIPLY",
	"DIVIDE",
	"LEFT",
	"RIGHT",
	"UP",
	"DOWN",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"PAUSE",
	"KEYCOUNT"
};

InputManager::InputManager() :
	m_canProcessInput(true), m_blockMouseInput(false),
	m_xSensitivity(0.f), m_ySensitivity(0.f),
	m_fixedMousePosition(0, 0)
{
	if (s_Instance)
	{
		Utility::Error("Pointer to class \'InputManager\' is invalid");
	}
	s_Instance = this;

	try
	{
		std::ifstream ifs_input("config/input.xml");
		cereal::XMLInputArchive input_config(ifs_input);

		input_config(m_configuration);
	}
	catch (cereal::Exception& ex)
	{
		spdlog::warn("Failed to load input configuration: {}, default values used", ex.what());

		m_configuration = InputConfiguration();

		std::ofstream ofs_input("config/input.xml");
		cereal::XMLOutputArchive input_config(ofs_input);

		input_config(m_configuration);
	}

	m_keyActionPairList.emplace_back(sKeyActionPair("forward", m_configuration.key_forward));
	m_keyActionPairList.emplace_back(sKeyActionPair("backward", m_configuration.key_backward));
	m_keyActionPairList.emplace_back(sKeyActionPair("strafel", m_configuration.key_strafel));
	m_keyActionPairList.emplace_back(sKeyActionPair("strafer", m_configuration.key_strafer));
	m_keyActionPairList.emplace_back(sKeyActionPair("jump", m_configuration.key_jump));
	m_keyActionPairList.emplace_back(sKeyActionPair("crouch", m_configuration.key_crouch));
	m_keyActionPairList.emplace_back(sKeyActionPair("sprint", m_configuration.key_sprint));
	m_keyActionPairList.emplace_back(sKeyActionPair("use", m_configuration.key_use));
	m_keyActionPairList.emplace_back(sKeyActionPair("reload", m_configuration.key_reload));
	m_keyActionPairList.emplace_back(sKeyActionPair("firemode", m_configuration.key_firemode));
	m_keyActionPairList.emplace_back(sKeyActionPair("inventory", m_configuration.key_inventory));
	m_keyActionPairList.emplace_back(sKeyActionPair("primary", m_configuration.key_primary));
	m_keyActionPairList.emplace_back(sKeyActionPair("secondary", m_configuration.key_secondary));

	m_xSensitivity = m_configuration.xsens;
	m_ySensitivity = m_configuration.ysens;
}


void InputManager::update(bool processInput)
{
	m_fixedMousePosition = irr::core::vector2df(
		static_cast<float>(RenderManager::Get()->getConfiguration().width / 2),
		static_cast<float>(RenderManager::Get()->getConfiguration().height / 2));

	if (!processInput)
	{
		m_canProcessInput = false;
	}

	if (m_canProcessInput)
	{
		for (auto& pair : m_keyActionPairList)
		{
			pair.pressed = isKeyPressed(KEYBOARD_KEY(pair.getKey()));
			pair.released = getKeyRelease(KEYBOARD_KEY(pair.getKey()), &pair.toggle);
		}
	}
}

bool InputManager::isActionPressed(std::string action)
{
	if (m_canProcessInput)
	{
		for (auto& pair : m_keyActionPairList)
		{
			if (pair.getAction() == action)
			{
				return pair.pressed;
			}
		}
	}

	return false;
}

bool InputManager::isActionReleased(std::string action)
{
	if (m_canProcessInput)
	{
		for (auto& pair : m_keyActionPairList)
		{
			if (pair.getAction() == action)
			{
				return pair.released;
			}
		}
	}

	return false;
}

bool InputManager::isKeyPressed(int key, bool ignore_process_flag)
{
	auto keycode = 0;

	switch (key)
	{
	default: keycode = 0;
		break;
	case KEY_A: keycode = 'A';
		break;
	case KEY_B: keycode = 'B';
		break;
	case KEY_C: keycode = 'C';
		break;
	case KEY_D: keycode = 'D';
		break;
	case KEY_E: keycode = 'E';
		break;
	case KEY_F: keycode = 'F';
		break;
	case KEY_G: keycode = 'G';
		break;
	case KEY_H: keycode = 'H';
		break;
	case KEY_I: keycode = 'I';
		break;
	case KEY_J: keycode = 'J';
		break;
	case KEY_K: keycode = 'K';
		break;
	case KEY_L: keycode = 'L';
		break;
	case KEY_M: keycode = 'M';
		break;
	case KEY_N: keycode = 'N';
		break;
	case KEY_O: keycode = 'O';
		break;
	case KEY_P: keycode = 'P';
		break;
	case KEY_Q: keycode = 'Q';
		break;
	case KEY_R: keycode = 'R';
		break;
	case KEY_S: keycode = 'S';
		break;
	case KEY_T: keycode = 'T';
		break;
	case KEY_U: keycode = 'U';
		break;
	case KEY_V: keycode = 'V';
		break;
	case KEY_W: keycode = 'W';
		break;
	case KEY_X: keycode = 'X';
		break;
	case KEY_Y: keycode = 'Y';
		break;
	case KEY_Z: keycode = 'Z';
		break;
	case KEY_NUM0: keycode = '0';
		break;
	case KEY_NUM1: keycode = '1';
		break;
	case KEY_NUM2: keycode = '2';
		break;
	case KEY_NUM3: keycode = '3';
		break;
	case KEY_NUM4: keycode = '4';
		break;
	case KEY_NUM5: keycode = '5';
		break;
	case KEY_NUM6: keycode = '6';
		break;
	case KEY_NUM7: keycode = '7';
		break;
	case KEY_NUM8: keycode = '8';
		break;
	case KEY_NUM9: keycode = '9';
		break;
	case KEY_ESCAPE: keycode = VK_ESCAPE;
		break;
	case KEY_LCONTROL: keycode = VK_LCONTROL;
		break;
	case KEY_LSHIFT: keycode = VK_LSHIFT;
		break;
	case KEY_LALT: keycode = VK_LMENU;
		break;
	case KEY_LSYSTEM: keycode = VK_LWIN;
		break;
	case KEY_RCONTROL: keycode = VK_RCONTROL;
		break;
	case KEY_RSHIFT: keycode = VK_RSHIFT;
		break;
	case KEY_RALT: keycode = VK_RMENU;
		break;
	case KEY_RSYSTEM: keycode = VK_RWIN;
		break;
	case KEY_MENU: keycode = VK_APPS;
		break;
	case KEY_LBRACKET: keycode = VK_OEM_4;
		break;
	case KEY_RBRACKET: keycode = VK_OEM_6;
		break;
	case KEY_SEMICOLON: keycode = VK_OEM_1;
		break;
	case KEY_COMMA: keycode = VK_OEM_COMMA;
		break;
	case KEY_PERIOD: keycode = VK_OEM_PERIOD;
		break;
	case KEY_QUOTE: keycode = VK_OEM_7;
		break;
	case KEY_SLASH: keycode = VK_OEM_2;
		break;
	case KEY_BACKSLASH: keycode = VK_OEM_5;
		break;
	case KEY_TILDE: keycode = VK_OEM_3;
		break;
	case KEY_EQUAL: keycode = VK_OEM_PLUS;
		break;
	case KEY_DASH: keycode = VK_OEM_MINUS;
		break;
	case KEY_SPACE: keycode = VK_SPACE;
		break;
	case KEY_RETURN: keycode = VK_RETURN;
		break;
	case KEY_BACKSPACE: keycode = VK_BACK;
		break;
	case KEY_TAB: keycode = VK_TAB;
		break;
	case KEY_PAGEUP: keycode = VK_PRIOR;
		break;
	case KEY_PAGEDOWN: keycode = VK_NEXT;
		break;
	case KEY_END: keycode = VK_END;
		break;
	case KEY_HOME: keycode = VK_HOME;
		break;
	case KEY_INSERT: keycode = VK_INSERT;
		break;
	case KEY_DELETE: keycode = VK_DELETE;
		break;
	case KEY_ADD: keycode = VK_ADD;
		break;
	case KEY_SUBTRACT: keycode = VK_SUBTRACT;
		break;
	case KEY_MULTIPLY: keycode = VK_MULTIPLY;
		break;
	case KEY_DIVIDE: keycode = VK_DIVIDE;
		break;
	case KEY_LEFT: keycode = VK_LEFT;
		break;
	case KEY_RIGHT: keycode = VK_RIGHT;
		break;
	case KEY_UP: keycode = VK_UP;
		break;
	case KEY_DOWN: keycode = VK_DOWN;
		break;
	case KEY_NUMPAD0: keycode = VK_NUMPAD0;
		break;
	case KEY_NUMPAD1: keycode = VK_NUMPAD1;
		break;
	case KEY_NUMPAD2: keycode = VK_NUMPAD2;
		break;
	case KEY_NUMPAD3: keycode = VK_NUMPAD3;
		break;
	case KEY_NUMPAD4: keycode = VK_NUMPAD4;
		break;
	case KEY_NUMPAD5: keycode = VK_NUMPAD5;
		break;
	case KEY_NUMPAD6: keycode = VK_NUMPAD6;
		break;
	case KEY_NUMPAD7: keycode = VK_NUMPAD7;
		break;
	case KEY_NUMPAD8: keycode = VK_NUMPAD8;
		break;
	case KEY_NUMPAD9: keycode = VK_NUMPAD9;
		break;
	case KEY_F1: keycode = VK_F1;
		break;
	case KEY_F2: keycode = VK_F2;
		break;
	case KEY_F3: keycode = VK_F3;
		break;
	case KEY_F4: keycode = VK_F4;
		break;
	case KEY_F5: keycode = VK_F5;
		break;
	case KEY_F6: keycode = VK_F6;
		break;
	case KEY_F7: keycode = VK_F7;
		break;
	case KEY_F8: keycode = VK_F8;
		break;
	case KEY_F9: keycode = VK_F9;
		break;
	case KEY_F10: keycode = VK_F10;
		break;
	case KEY_F11: keycode = VK_F11;
		break;
	case KEY_F12: keycode = VK_F12;
		break;
	case KEY_F13: keycode = VK_F13;
		break;
	case KEY_F14: keycode = VK_F14;
		break;
	case KEY_F15: keycode = VK_F15;
		break;
	case KEY_PAUSE: keycode = VK_PAUSE;
		break;
	case KEY_MOUSE0: keycode = 0xFFF1;
		break;
	case KEY_MOUSE1: keycode = 0xFFF2;
		break;
	case KEY_MOUSE_MID: keycode = 0xFFF3;
		break;
	case KEY_MOUSE_X1: keycode = 0xFFF4;
		break;
	case KEY_MOUSE_X2: keycode = 0xFFF5;
		break;
	}

	switch (keycode)
	{
	case 0xFFF1:
		return m_canProcessInput || ignore_process_flag ? isMouseButtonPressed(0) : false;
	case 0xFFF2:
		return m_canProcessInput || ignore_process_flag ? isMouseButtonPressed(1) : false;
	case 0xFFF3:
		return m_canProcessInput || ignore_process_flag ? isMouseButtonPressed(2) : false;
	case 0xFFF4:
		return m_canProcessInput || ignore_process_flag ? isMouseButtonPressed(3) : false;
	case 0xFFF5:
		return m_canProcessInput || ignore_process_flag ? isMouseButtonPressed(4) : false;
	default:
		return m_canProcessInput || ignore_process_flag ? (GetAsyncKeyState(keycode) & 0x8000) > 0 : false;
	}
}


bool InputManager::getKeyPressOnce(int key, bool* state, bool ignore_process_flag)
{
	if (m_canProcessInput || ignore_process_flag)
	{
		if (isKeyPressed(KEYBOARD_KEY(key), ignore_process_flag) && !*state)
		{
			*state = true;
			return true;
		}

		if (!isKeyPressed(KEYBOARD_KEY(key), ignore_process_flag) && *state)
		{
			*state = false;
			return false;
		}
	}

	return false;
}

bool InputManager::getKeyRelease(int key, bool* state, bool ignore_process_flag)
{
	if (m_canProcessInput || ignore_process_flag)
	{
		if (isKeyPressed(KEYBOARD_KEY(key), ignore_process_flag))
		{
			*state = true;
			return false;
		}

		if (!isKeyPressed(KEYBOARD_KEY(key), ignore_process_flag) && *state)
		{
			*state = false;
			return true;
		}
	}

	return false;
}

bool InputManager::getMousePressOnce(int button, bool* state, bool ignore_process_flag)
{
	if (m_canProcessInput || ignore_process_flag)
	{
		if (isMouseButtonPressed(MOUSE_BUTTON(button), ignore_process_flag) && !*state)
		{
			*state = true;
			return true;
		}

		if (!isMouseButtonPressed(MOUSE_BUTTON(button), ignore_process_flag) && *state)
		{
			*state = false;
			return false;
		}
	}

	return false;
}

bool InputManager::getMouseRelease(int button, bool* state, bool ignore_process_flag)
{
	if (m_canProcessInput || ignore_process_flag)
	{
		if (isMouseButtonPressed(MOUSE_BUTTON(button), ignore_process_flag))
		{
			*state = true;
			return false;
		}

		if (!isMouseButtonPressed(MOUSE_BUTTON(button), ignore_process_flag) && *state)
		{
			*state = false;
			return true;
		}
	}

	return false;
}

#undef MB_RIGHT
bool InputManager::isMouseButtonPressed(int button, bool ignore_process_flag)
{
	auto button_code = 0;

	switch (button)
	{
	case MB_LEFT: button_code = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
		break;
	case MB_RIGHT: button_code = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
		break;
	case MB_MIDDLE: button_code = VK_MBUTTON;
		break;
	case MB_XBUTTON1: button_code = VK_XBUTTON1;
		break;
	case MB_XBUTTON2: button_code = VK_XBUTTON2;
		break;
	default: button_code = 0;
		break;
	}

	if (m_blockMouseInput && !ignore_process_flag)
	{
		return false;
	}

	return (m_canProcessInput || ignore_process_flag) ? (GetAsyncKeyState(button_code) & 0x8000) != 0 : false;
}

irr::core::vector2df InputManager::getMouseDelta()
{
	if (m_canProcessInput)
	{
		auto temp = irr::core::vector2df(m_fixedMousePosition.X, m_fixedMousePosition.Y) - getMousePosition();
		auto delta =
			irr::core::vector2df(temp.X, temp.Y);
		centerMouse();
		return delta;
	}

	return irr::core::vector2df(0, 0);
}

irr::core::vector2df InputManager::getMousePosition()
{
	POINT point;
	GetCursorPos(&point);
	return irr::core::vector2df(static_cast<float>(point.x), static_cast<float>(point.y));
}

irr::core::vector2df InputManager::getMousePositionWindow(/*HWND window*/)
{
	return irr::core::vector2df();
}

void InputManager::setMousePosition(irr::core::vector2df position)
{
	SetCursorPos(static_cast<int>(position.X), static_cast<int>(position.Y));
}

void InputManager::setMousePositionWindow(irr::core::vector2df position/*, HWND relativeTo*/)
{
	// TODO: Set mouse position in active window
}

void InputManager::centerMouse()
{
	setMousePosition(m_fixedMousePosition);
}

bool InputManager::canProcessInput(bool process)
{
	m_canProcessInput = process;
	return m_canProcessInput;
}

bool InputManager::blockMouseInput(bool process)
{
	m_blockMouseInput = process;
	return m_blockMouseInput;
}

void InputManager::saveConfiguration(InputConfiguration configuration)
{
	std::ofstream ofs_input("config/input.xml");
	cereal::XMLOutputArchive input_config(ofs_input);

	m_configuration = configuration;
	input_config(configuration);
}
