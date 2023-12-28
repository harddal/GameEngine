#include "Bindings.h"

#include <string>

#include "Engine/Engine.h"
#include "Utility/Utility.h"

// DEBUG
static void AS_Debug_Warning(std::string& str) { Utility::Warning(str); }
static void AS_Debug_Error(std::string& str) { Utility::Error(str); }

// LOG
static void AS_Log_Info(std::string& str) { spdlog::info(str); }
static void AS_Log_Debug(std::string& str) { spdlog::debug(str); }
static void AS_Log_Warning(std::string& str) { spdlog::warn(str); }
static void AS_Log_Error(std::string& str) { spdlog::error(str); }

// MATH
static int AS_Random_Int(irr::u32 n) { return rand() % n + 1; }
static irr::core::vector3df AS_Normalize_Vector3(irr::core::vector3df& v) { return v.normalize(); }

// STRING
static std::string AS_String_To_String_Int(irr::u32& in) { return std::to_string(in); }
static std::string AS_String_To_String_Float(irr::f32& in) { return std::to_string(in); }
static int AS_Int_To_String(std::string& in) { return atoi(in.c_str()); }
static float AS_Float_To_String(std::string& in) { return static_cast<float>(atof(in.c_str())); }

// TIME
static irr::u32 AS_Get_Time_Current() { return Engine::Get()->getCurrentTime(); }
static irr::f32 AS_Get_Time_Delta() { return Engine::Get()->getDeltaTime(); }

void ScriptBindings::RegisterCore(asIScriptEngine* engine)
{
    engine->SetDefaultNamespace("debug");
    {
        engine->RegisterGlobalFunction("void warning(string &in)", asFUNCTION(AS_Debug_Warning), asCALL_CDECL);
        engine->RegisterGlobalFunction("void error(string &in)", asFUNCTION(AS_Debug_Error), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("log");
    {
        engine->RegisterGlobalFunction("void info(string &in)", asFUNCTION(AS_Log_Info), asCALL_CDECL);
        engine->RegisterGlobalFunction("void debug(string &in)", asFUNCTION(AS_Log_Debug), asCALL_CDECL);
        engine->RegisterGlobalFunction("void warning(string &in)", asFUNCTION(AS_Log_Warning), asCALL_CDECL);
        engine->RegisterGlobalFunction("void error(string &in)", asFUNCTION(AS_Log_Error), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("math"); 
    {
        engine->RegisterGlobalFunction("int random(int)", asFUNCTION(AS_Random_Int), asCALL_CDECL);
        engine->RegisterGlobalFunction("vector3d normalize(vector3d)", asFUNCTION(AS_Normalize_Vector3), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("string");
    {
        engine->RegisterGlobalFunction("string to_string(int &in)", asFUNCTION(AS_String_To_String_Int), asCALL_CDECL);
        engine->RegisterGlobalFunction("string to_string(float &in)", asFUNCTION(AS_String_To_String_Float), asCALL_CDECL);
        engine->RegisterGlobalFunction("int to_int(string &in)", asFUNCTION(AS_Int_To_String), asCALL_CDECL);
        engine->RegisterGlobalFunction("float to_float(string &in)", asFUNCTION(AS_Float_To_String), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("time");
    {
        engine->RegisterGlobalFunction("int get()", asFUNCTION(AS_Get_Time_Current), asCALL_CDECL);
        engine->RegisterGlobalFunction("int getDelta()", asFUNCTION(AS_Get_Time_Delta), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("");
}
