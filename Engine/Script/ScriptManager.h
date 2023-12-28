#pragma once

#include "Engine/World/Components/ScriptComponent.h"

#include "angelscript.h"

#include "Engine/Script/Bindings.h"

class ScriptManager
{
public:
    ScriptManager();
    ~ScriptManager();

    int execute(std::string module, std::string function, int entid = 0xFFFF);
    // TODO: Change the ScriptComponent parameter to accept a module name string instead
    int execute(ScriptComponent script, asIScriptFunction* function, int entid = 0xFFFF);

    int compile(std::string filename, std::string module);
    int compile(ScriptComponent& script);

    int removeModule(std::string module) const { return m_engine->DiscardModule(module.c_str()); }

    asIScriptEngine* getEngine() { return m_engine; }

    static ScriptManager* Get() { return s_Instance; }

private:
    static ScriptManager* s_Instance;

    static void MessageCallback(const asSMessageInfo* msg, void* param);

    void ConfigureEngine(asIScriptEngine* engine);

    static void LineCallback(asIScriptContext* ctx, unsigned long* timeOut) { if (*timeOut < 1000/*timeGetTime()*/) { ctx->Abort(); } };

    asIScriptEngine* m_engine;
    asIScriptContext* m_executionContext;
};
