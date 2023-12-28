#pragma once

#include <string>

#include "anax/Component.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include <angelscript.h>
#include <vector2d.h>
#include <vector3d.h>

#define _entity_script_init    "void init(int)"
#define _entity_script_update  "void update(int)"
#define _entity_script_destroy "void destroy(int)"

#define _entity_script_on_interaction "void onInteraction(int)"
#define _entity_script_on_kill_event  "void onKill(int)"
#define _entity_script_on_use_event   "void onUse(int)"
#define _entity_script_on_logic_event "void onLogicEvent(int)"

enum class AS_DATA_TYPE
{
    INT,
    BOOL,
    FLOAT,
    STRING,
    VECTOR2,
    VECTOR3,
    NO_TYPE
};

struct ExposedScriptData
{
    unsigned int index;

    std::string data, declaration;

    AS_DATA_TYPE type;

    ExposedScriptData() : index(0), type(AS_DATA_TYPE::NO_TYPE) {}
    ExposedScriptData(unsigned int index, std::string declaration, AS_DATA_TYPE type, std::string data)
    {
        this->index = index;
        this->declaration = declaration;
        this->type = type;
        this->data = data;
    }
};

struct ScriptComponent : anax::Component
{
    std::string script;

    friend class ScriptManager;
    friend class ScriptSystem;
    friend class WorldManager;

    bool
        active,
        initialized,
    // DEPRECATED -- Check if script function is nullptr instead
        hasInit = false,
        hasUpdate = false,
        hasDestroy = false,
		hasOnInteraction = false,
		hasOnKillEventFunc = false,
        hasOnUseEventFunc = false,
        hasOnLogicEventActivate = false; 

    std::string module;

    asIScriptFunction 
		*initFunc, 
        *updateFunc, 
        *destroyFunc,
		*onPlayerInteractionFunc, 
        *onKillEventFunc,
        *onUseEventFunc,
        *onLogicEventActivate;

    asIScriptContext* context;

    std::vector<std::pair<int, std::string>> globals;
    std::vector<std::pair<AS_DATA_TYPE, void*>> global_values;
    std::vector<int> exposed_global_indexes;

    std::vector<ExposedScriptData> script_data;

    template <class Archive>
    void serialize(Archive& archive)
    {
        std::vector<std::string> data;

        for (auto i = 0U; i < globals.size(); i++) {
            std::string data_str;

            switch (global_values.at(i).first) {
            case AS_DATA_TYPE::INT:
            {
                auto *value = reinterpret_cast<int*>(global_values.at(i).second);
                data_str = std::to_string(*value);
                break;
            }
            case AS_DATA_TYPE::BOOL:
            {
                auto *value = reinterpret_cast<bool*>(global_values.at(i).second);
                data_str = std::to_string(*value);
                break;
            }
            case AS_DATA_TYPE::FLOAT:
            {
                auto *value = reinterpret_cast<float*>(global_values.at(i).second);
                data_str = std::to_string(*value);
                break;
            }
            case AS_DATA_TYPE::STRING:
            {
                auto *value = reinterpret_cast<std::string*>(global_values.at(i).second);
                data_str = *value;
                break;
            }
            case AS_DATA_TYPE::VECTOR2:
            {
                auto *value = reinterpret_cast<irr::core::vector2df*>(global_values.at(i).second);
                data_str = std::to_string(value->X) + "," + std::to_string(value->Y);
                break;
            }
            case AS_DATA_TYPE::VECTOR3:
            {
                auto *value = reinterpret_cast<irr::core::vector3df*>(global_values.at(i).second);
                data_str = std::to_string(value->X) + "," + std::to_string(value->Y) + "," + std::to_string(value->Z);
                break;
            }
            default:
                break;
            }

            // INDEX : TYPE : DATA
            data.emplace_back(
                std::to_string(globals.at(i).first) + ":" + std::to_string(static_cast<unsigned int>(global_values.at(i).first)) + ":" + data_str);
        }

        archive(CEREAL_NVP(script), CEREAL_NVP(data));
    }

    ScriptComponent() : 
        active(false), initialized(false),
        initFunc(nullptr),updateFunc(nullptr), destroyFunc(nullptr), onPlayerInteractionFunc(nullptr), onKillEventFunc(nullptr), onUseEventFunc(nullptr), onLogicEventActivate(nullptr),
        context(nullptr) {}
};
