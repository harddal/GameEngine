#pragma once

#include "angelscript.h"

class ScriptBindings
{
public:
    static void RegisterBindings(asIScriptEngine* engine)
    {
        RegisterVector2d(engine);
        RegisterVector3d(engine);

        RegisterCore(engine);
        RegisterEntity(engine);
        RegisterInputManager(engine);
        RegisterPhysX(engine);
        RegisterRender(engine);
		RegisterSound(engine);
		RegisterGame(engine);
    }

private:
    static void RegisterVector2d(asIScriptEngine* engine);
    static void RegisterVector3d(asIScriptEngine* engine);

    static void RegisterCore(asIScriptEngine* engine);
    static void RegisterInputManager(asIScriptEngine* engine);
    static void RegisterEntity(asIScriptEngine* engine);
    static void RegisterPhysX(asIScriptEngine* engine);
    static void RegisterRender(asIScriptEngine* engine);
	static void RegisterSound(asIScriptEngine* engine);
	static void RegisterGame(asIScriptEngine* engine);
};
