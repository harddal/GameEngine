#include "Bindings.h"

#include "Engine/Engine.h"

void ScriptBindings::RegisterInputManager(asIScriptEngine *engine)
{
    engine->RegisterObjectType("InputManager", 0, asOBJ_REF | asOBJ_NOHANDLE);
    engine->RegisterGlobalProperty("InputManager input", InputManager::Get());

    engine->RegisterObjectMethod("InputManager", "bool onActionPress(string)", asMETHOD(InputManager, isActionPressed), asCALL_THISCALL);
    engine->RegisterObjectMethod("InputManager", "bool onActionRelease(string)", asMETHOD(InputManager, isActionReleased), asCALL_THISCALL);

    engine->RegisterObjectMethod("InputManager", "vector2d getMousePosition()", asMETHOD(InputManager, getMousePosition), asCALL_THISCALL);
    engine->RegisterObjectMethod("InputManager", "vector2d getMouseDelta()", asMETHOD(InputManager, getMouseDelta), asCALL_THISCALL);

    engine->RegisterObjectMethod("InputManager", "bool getMousePress(int)", asMETHOD(InputManager, isMouseButtonPressed_ASBinding), asCALL_THISCALL);

    engine->RegisterObjectMethod("InputManager", "void centerMouse()", asMETHOD(InputManager, centerMouse), asCALL_THISCALL);
}