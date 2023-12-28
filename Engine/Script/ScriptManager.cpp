#include "ScriptManager.h"

#include <memory>

#include <spdlog/spdlog.h>

#include "angelscript/sdk/add_on/scriptstdstring/scriptstdstring.h"
#include "angelscript/sdk/add_on/scriptarray/scriptarray.h"
#include "angelscript/sdk/add_on/scriptbuilder/scriptbuilder.h"
#include "angelscript/sdk/add_on/scriptmath/scriptmath.h"
#include "angelscript/sdk/add_on/scriptmath/scriptmathcomplex.h"
#include "angelscript/sdk/add_on/scriptfile/scriptfile.h"
#include "angelscript/sdk/add_on/scriptfile/scriptfilesystem.h"

#include "Engine/Resource/FilePaths.h"

#include "Engine/World/Components/ScriptComponent.h"
#include "Engine/World/Components/TransformComponent.h"


//#define AS_SCRIPT_FULL_OPTIMIZE

using namespace std;

ScriptManager* ScriptManager::s_Instance = nullptr;

ScriptManager::ScriptManager()
{
    if (s_Instance) { Utility::Error("Pointer to class \'ScriptManager\' is invalid"); }
    s_Instance = this;

    m_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (!m_engine) { Utility::Error("Failed to create the script engine"); }
    spdlog::info("AngelScript Version {}", ANGELSCRIPT_VERSION_STRING);

    m_engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

    ConfigureEngine(m_engine);

    m_executionContext = m_engine->CreateContext();
    if (!m_executionContext) { Utility::Error("Failed to create the global script context"); }
}

ScriptManager::~ScriptManager()
{
    m_executionContext->Release();
    m_engine->ShutDownAndRelease();

    delete s_Instance;
}

// May not work at all, need to refactor
int ScriptManager::execute(std::string module, std::string function, int entid)
{
    int r = 0;

    asIScriptFunction* func = m_engine->GetModule(module.c_str())->GetFunctionByDecl(function.c_str());
    /*if (func == 0) {

		spdlog::warn("ScriptManager - Script Compile Error: Main entry point not defined in script module [" + module + "]", LOG_WARNING);
        return 0;
    }*/

    r = m_executionContext->Prepare(func);
    if (r < 0)
    {
        spdlog::warn("Failed to prepare the script context");
        return 0;
    }
    m_executionContext->SetArgDWord(0, entid);
    r = m_executionContext->Execute();
    if (r != asEXECUTION_FINISHED)
    {
        if (r == asEXECUTION_ABORTED)
        {
            spdlog::error("--> The script module [" + module + "] was aborted before it could finish");
        }
        else if (r == asEXECUTION_EXCEPTION)
        {
            spdlog::error("The script module [" + module + "] ended with an exception");

            func = m_executionContext->GetExceptionFunction();
            spdlog::debug("    func: " + string(func->GetDeclaration()));
            spdlog::debug("    modl: " + string(func->GetModuleName()));
            spdlog::debug("    sect: " + string(func->GetScriptSectionName()));
            spdlog::debug("    line: " + to_string(m_executionContext->GetExceptionLineNumber()));
            spdlog::debug("    desc: " + string(m_executionContext->GetExceptionString()));
        }
        else
            spdlog::error(
                "--> The script module [" + module + "] ended for some unforeseen reason (" + to_string(r) +
                string(")"));
    }
    else { auto returnValue = m_executionContext->GetReturnFloat(); }

    m_executionContext->Release();

    return 0;
}

int ScriptManager::execute(ScriptComponent script, asIScriptFunction* function, int entid)
{
    /*if (!function) {

        spdlog::warn("ScriptManager - Script Execute Error: Main entry point not defined in script module [" + script.module + "]", LOG_ERROR);
        return 0;
    }*/

    auto r = m_executionContext->Prepare(function);
    if (r == asNO_FUNCTION)
    {
        spdlog::error(
            "ScriptManager - Failed to create the script context for module \'" + script.module +
            "\'. Script function pointer was null");
    }
    if (r == asCONTEXT_ACTIVE)
    {
        spdlog::error(
            "ScriptManager - Failed to create the script context for module \'" + script.module +
            "\'. Context is already active/suspended");
    }
    if (r == asINVALID_ARG)
    {
        spdlog::error(
            "ScriptManager - Failed to create the script context for module \'" + script.module +
            "\'. Invalid function argument");
    }
    if (r == asOUT_OF_MEMORY)
    {
        spdlog::error(
            "ScriptManager - Failed to create the script context for module \'" + script.module +
            "\'. Context ran out of memory allocating script function call stack");
    }

    m_executionContext->SetArgDWord(0, entid);
    r = m_executionContext->Execute();

    if (r != asEXECUTION_FINISHED)
    {
        if (r == asEXECUTION_ABORTED)
        {
            spdlog::debug("--> The script module [" + script.module + "] was aborted before it could finish");
        }
        else if (r == asEXECUTION_EXCEPTION)
        {
            spdlog::warn("--> The script module [" + script.module + "] ended with an exception");

            auto func = m_executionContext->GetExceptionFunction();
            spdlog::debug("    func: " + string(func->GetDeclaration()));
            spdlog::debug("    modl: " + string(func->GetModuleName()));
            spdlog::debug("    sect: " + string(func->GetScriptSectionName()));
            spdlog::debug("    line: " + to_string(m_executionContext->GetExceptionLineNumber()));
            spdlog::debug("    desc: " + string(m_executionContext->GetExceptionString()));
        }
        else
            spdlog::debug(
                "--> The script module [" + script.module + "] ended for some unforeseen reason (" + to_string(r) +
                string(")"));
    }

    return 0;
}

void ScriptManager::MessageCallback(const asSMessageInfo* msg, void* param)
{
    const char* type = "Script Exception";
    if (msg->type == asMSGTYPE_WARNING) { type = "Script Warning"; }
    else
        if (msg->type == asMSGTYPE_INFORMATION) { type = "Script Info"; }

    spdlog::error(
        string(msg->section) + "(" + to_string(msg->row) + ", " + to_string(msg->col) + ") : " + string(type) + " : " +
        string(msg->message));
}

void ScriptManager::ConfigureEngine(asIScriptEngine* engine)
{
#ifdef AS_SCRIPT_FULL_OPTIMIZE
	engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);
#endif

    RegisterScriptArray(engine, true);

    RegisterScriptMath(engine);
    RegisterScriptMathComplex(engine);

    RegisterStdString(engine);
    RegisterStdStringUtils(engine);

    RegisterScriptFile(engine);
    RegisterScriptFileSystem(engine);

    ScriptBindings::RegisterBindings(engine);
}

int ScriptManager::compile(std::string filename, std::string module)
{
    int r;

    CScriptBuilder builder;

    r = builder.StartNewModule(m_engine, module.c_str());
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to start new module [" + module + "]");
        return r;
    }

    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to add script file " + filename);
        return r;
    }

    r = builder.BuildModule();
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to build the module [" + module + "]");
        return r;
    }

    return r;
}

int ScriptManager::compile(ScriptComponent& script)
{
    int r;

    CScriptBuilder builder;

    r = builder.StartNewModule(m_engine, script.module.c_str());
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to start new module [" + script.module + "]");
        return r;
    }

    r = builder.AddSectionFromFile(std::string(_asset_asc(script.script)).c_str());
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to add script file " + std::string(_asset_asc(script.script)));
        return r;
    }

    r = builder.BuildModule();
    if (r < 0)
    {
        spdlog::warn("ScriptManager - Failed to build the module [" + script.module + "]");
        return r;
    }

    // Populate the list of global vars index and names
    script.globals.clear();
    std::vector<std::pair<int, std::string>>().swap(script.globals);
    script.global_values.clear();
    std::vector<std::pair<AS_DATA_TYPE, void*>>().swap(script.global_values);
    script.exposed_global_indexes.clear();
    std::vector<int>().swap(script.exposed_global_indexes);

    int count = m_engine->GetModule(script.module.c_str())->GetGlobalVarCount();
    for (auto n = 0; n < count; n++)
    {
        auto decl = string(builder.GetModule()->GetGlobalVarDeclaration(n));
        script.globals.emplace_back(pair<int, string>(n, decl));

        if (!builder.GetMetadataForVar(n).empty()) {
            std::string meta_data = builder.GetMetadataForVar(n).back(); // DEBUG: Function changed may cause crash
            if (meta_data == "export") { script.exposed_global_indexes.emplace_back(n); }
        }

        auto type = decl.substr(0, decl.find_first_of(' '));

        AS_DATA_TYPE e_type;
        if (type == "int") { e_type = AS_DATA_TYPE::INT; }
        if (type == "bool") { e_type = AS_DATA_TYPE::BOOL; }
        if (type == "float") { e_type = AS_DATA_TYPE::FLOAT; }
        if (type == "string") { e_type = AS_DATA_TYPE::STRING; }
        if (type == "vector2d") { e_type = AS_DATA_TYPE::VECTOR2; }
        if (type == "vector3d") { e_type = AS_DATA_TYPE::VECTOR3; }

        // CRITICAL *** DEBUG: Storing pointers to script vars could cause odd crashes
        // DEBUG: With unqiue module names this problem has been 'solved' I think, still watching it though
        script.global_values.emplace_back(
            std::pair<AS_DATA_TYPE, void*>(e_type, builder.GetModule()->GetAddressOfGlobalVar(n)));
    }

    if (!script.script_data.empty())
    {
        for (auto ex_data : script.script_data)
        {
            switch (ex_data.type)
            {
            case AS_DATA_TYPE::INT:
                {
                    auto* value = reinterpret_cast<int*>(script.global_values.at(ex_data.index).second);
                    *value = atoi(ex_data.data.c_str());
                    break;
                }
            case AS_DATA_TYPE::BOOL:
                {
                    auto* value = reinterpret_cast<bool*>(script.global_values.at(ex_data.index).second);
                    *value = atoi(ex_data.data.c_str());
                    break;
                }
            case AS_DATA_TYPE::FLOAT:
                {
                    auto* value = reinterpret_cast<float*>(script.global_values.at(ex_data.index).second);
                    *value = static_cast<float>(atof(ex_data.data.c_str()));
                    break;
                }
            case AS_DATA_TYPE::STRING:
                {
                    auto* value = reinterpret_cast<std::string*>(script.global_values.at(ex_data.index).second);
                    *value = ex_data.data;
                    break;
                }
            case AS_DATA_TYPE::VECTOR2:
                {
                    auto* value = reinterpret_cast<irr::core::vector2df*>(script.global_values.at(ex_data.index).second
                    );
                    value->X = static_cast<float>(atof(ex_data.data.substr(0, ex_data.data.find_first_of(',')).c_str()));
                    value->Y = static_cast<float>(atof(ex_data.data.substr(ex_data.data.find_first_of(',') + 1).c_str()));
                    break;
                }
            case AS_DATA_TYPE::VECTOR3:
                {
                    auto* value = reinterpret_cast<irr::core::vector3df*>(script.global_values.at(ex_data.index).second
                    );
                    value->X = static_cast<float>(atof(ex_data.data.substr(0, ex_data.data.find_first_of(',')).c_str()));
                    value->Y = static_cast<float>(atof(
                        ex_data.data.substr(ex_data.data.find_first_of(',') + 1, ex_data.data.find_last_of(',')).
                                c_str()));
                    value->Z = static_cast<float>(atof(ex_data.data.substr(ex_data.data.find_last_of(',') + 1).c_str()));
                    break;
                }
            default:
                break;
            }
        }
    }

    if (r >= 0)
    {
        auto* func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_init);
        if (func)
        {
            script.hasInit = true;
            script.initFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_update);
        if (func)
        {
            script.hasUpdate = true;
            script.updateFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_destroy);
        if (func)
        {
            script.hasDestroy = true;
            script.destroyFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_on_interaction);
        if (func)
        {
            script.hasOnInteraction = true;
            script.onPlayerInteractionFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_on_kill_event);
        if (func)
        {
            script.hasOnKillEventFunc = true;
            script.onKillEventFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_on_use_event);
        if (func)
        {
            script.hasOnUseEventFunc = true;
            script.onUseEventFunc = func;
        }

        func = m_engine->GetModule(script.module.c_str())->GetFunctionByDecl(_entity_script_on_logic_event);
        if (func)
        {
            script.hasOnLogicEventActivate = true;
            script.onLogicEventActivate = func;
        }

        script.context = m_engine->CreateContext();
        if (!script.context) { spdlog::error("Failed to create context for script module: " + script.module); }
    }

    return r;
}
