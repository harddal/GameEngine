#pragma once

#include <vector>
#include <vector2d.h>
#include <cereal/cereal.hpp>

#include "Engine/Input/InputMap.h"

struct InputConfiguration
{
    irr::u32 
        key_forward,
        key_backward,
        key_strafel,
        key_strafer,
        key_jump,
        key_crouch,
        key_sprint,
        key_use,
        key_reload,
		key_firemode,
        key_inventory,
		key_primary,
		key_secondary;

    irr::f32
        xsens,
        ysens;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(key_forward),
            CEREAL_NVP(key_backward),
            CEREAL_NVP(key_strafel),
            CEREAL_NVP(key_strafer),
            CEREAL_NVP(key_jump),
            CEREAL_NVP(key_crouch),
            CEREAL_NVP(key_sprint),
            CEREAL_NVP(key_use),
            CEREAL_NVP(key_reload),
			CEREAL_NVP(key_firemode),
            CEREAL_NVP(key_inventory),
			CEREAL_NVP(key_primary),
			CEREAL_NVP(key_secondary),
            CEREAL_NVP(xsens),
            CEREAL_NVP(ysens));
    }

    InputConfiguration() : 
        key_forward(22),
        key_backward(18),
        key_strafel(0),
        key_strafer(3),
        key_jump(57),
        key_crouch(2),
        key_sprint(38),
        key_use(4),
        key_reload(17),
		key_firemode(23),
        key_inventory(8),
		key_primary(101),
		key_secondary(102),
        xsens(0.2f),
        ysens(0.2f)
    {}
};

struct sKeyActionPair
{
    bool pressed;
    bool released;
    bool toggle;

    std::string action;
    int key;

public:
    int getKey() const { return key; }

    std::string getAction() const { return action; }

    bool isPressed(std::string act)  const { return action == act && pressed  ? true : false; }
    bool isReleased(std::string act) const { return action == act && released ? true : false; }

    sKeyActionPair() :
        pressed(false), released(false), toggle(false), action(std::string()), key(0) {}

    sKeyActionPair(std::string action, int key) :
        pressed(false), released(false), toggle(false)
    {
        this->action = action;
        this->key = key;
    }
};

class InputManager
{
public:
	InputManager& operator=(const InputManager&) = delete;

    InputManager();
    ~InputManager() { delete s_Instance; }

	void update(bool processInput = true);

	bool isActionPressed(std::string action);
	bool isActionReleased(std::string action);

	bool isKeyPressed(int key, bool ignore_process_flag = false);

	bool getKeyPressOnce(int key, bool *state, bool ignore_process_flag = false);
	bool getKeyRelease(int key, bool *state, bool ignore_process_flag = false);

	bool isMouseButtonPressed(int button, bool ignore_process_flag = false);
	bool isMouseButtonPressed_ASBinding(int button) { return isMouseButtonPressed(button); }
    
	bool getMousePressOnce(int button, bool *state, bool ignore_process_flag = false);
	bool getMouseRelease(int button, bool *state, bool ignore_process_flag = false);

	irr::core::vector2df getMousePosition();
	irr::core::vector2df getMousePositionWindow(/*HWND window*/);

	void setMousePosition(irr::core::vector2df position);
	void setMousePositionWindow(irr::core::vector2df position/*, HWND window*/);

    void centerMouse();

	irr::core::vector2df getMouseDelta();

    bool canProcessInput(bool process = false);
    bool blockMouseInput(bool process = true);

    InputConfiguration getConfiguration() const { return m_configuration; }
	void saveConfiguration(InputConfiguration configuration);

	std::vector<sKeyActionPair>& getKeyActionPairList() { return m_keyActionPairList; }

    static InputManager* Get() { return s_Instance; }

private:
    static InputManager* s_Instance;

	bool m_canProcessInput, m_blockMouseInput;

    InputConfiguration m_configuration;

	std::vector<sKeyActionPair> m_keyActionPairList;

	double m_xSensitivity, m_ySensitivity;

	irr::core::vector2df m_fixedMousePosition;

};
