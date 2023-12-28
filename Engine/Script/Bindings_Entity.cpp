#include "Bindings.h"

#include <anax/anax.hpp>

#include "Engine/Engine.h"
#include "ScriptExceptions.h"
#include "Engine/Resource/FilePaths.h"

static irr::core::vector3df AS_Get_Entity_Position(entityid e)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            return entity.getComponent<TransformComponent>().getPosition();

        throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Get_Entity_Position"));
    }

    return irr::core::vector3df();
}

static irr::core::vector3df AS_Get_Entity_Rotation(entityid e)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            return entity.getComponent<TransformComponent>().getRotation();

        throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Get_Entity_Rotation"));
    }

    return irr::core::vector3df();
}

static irr::core::vector3df AS_Get_Entity_Scale(entityid e)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            return entity.getComponent<TransformComponent>().getScale();

        throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Get_Entity_Scale"));
    }

    return irr::core::vector3df();
}

static void AS_Set_Entity_Position(entityid e, irr::core::vector3df v)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            entity.getComponent<TransformComponent>().setPosition(v);
        else
            throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Set_Entity_Position"));
    }
}

static void AS_Set_Entity_Rotation(entityid e, irr::core::vector3df v)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            entity.getComponent<TransformComponent>().setRotation(v);
        else
            throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Set_Entity_Rotation"));
    }
}

static void AS_Set_Entity_Scale(entityid e, irr::core::vector3df v)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>())
            entity.getComponent<TransformComponent>().setScale(v);
        else
            throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Set_Entity_Scale"));
    }
}

static void AS_Move_Entity(entityid e, irr::core::vector3df v)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>()) {
            entity.getComponent<TransformComponent>().setPosition(entity.getComponent<TransformComponent>().getPosition() + v);
        }
        else
            throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) 
    {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Move_Entity"));
    }
}

static void AS_Rotate_Entity(entityid e, irr::core::vector3df v)
{
    try {
        auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

        if (!entity.isValid())
            throw ex_ent_invalid_name;

        if (entity.hasComponent<TransformComponent>()) {
            entity.getComponent<TransformComponent>().setRotation(entity.getComponent<TransformComponent>().getRotation() + v);
        }
        else
            throw ex_ent_invalid_comp;
    }
    catch (std::exception& ex) {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(e) + "\'" + std::string(" Function: ScriptBindings::AS_Rotate_Entity"));
    }
}

static std::string AS_Get_Entity_Name_By_ID(entityid id) { return WorldManager::Get()->managerSystem()->getNameByID(id); }
static int AS_Get_Entity_ID_By_Name(std::string& name) { return WorldManager::Get()->managerSystem()->getIDByName(name); }

static void AS_Set_Entity_Parent(entityid child, entityid parent)
{
    try {
        auto& child_entity = WorldManager::Get()->managerSystem()->getEntityByID(child);
        if (!child_entity.isValid())
            throw ex_ent_invalid_name;

        auto& parent_entity = WorldManager::Get()->managerSystem()->getEntityByID(parent);
        if (!parent_entity.isValid())
            throw ex_ent_invalid_name;

        if (child_entity.hasComponent<TransformComponent>() &&
            parent_entity.hasComponent<TransformComponent>()) {
			parent_entity.getComponent<TransformComponent>().addChild(child_entity.getComponent<TransformComponent>().node);
        }
        else {
            throw ex_ent_invalid_comp;
        }
    }
    catch (std::exception& ex)
    {
        spdlog::error(std::string(ex.what()) + "\'" + std::to_string(child) + ", " + std::to_string(parent) + "\'" + " Failed to set entity parent in: ScriptBindings::AS_Set_Entity_Parent");
    }
}

static void AS_Set_Camera_To_Entity(entityid child)
{
	try {
		auto& child_entity = WorldManager::Get()->managerSystem()->getEntityByID(child);
		if (!child_entity.isValid())
		{
			throw ex_ent_invalid_name;
		}

		if (child_entity.hasComponent<TransformComponent>())
		{
			auto& player = WorldManager::Get()->managerSystem()->getEntityByName("player");

			if (!player.isValid())
			{
				player = WorldManager::Get()->managerSystem()->getEntityByName("freecamera");
				if (!player.isValid())
				{
					throw ex_ent_invalid_name;
				}
			}

			//player.getComponent<TransformComponent>().addChild(child_entity.getComponent<TransformComponent>().node);

			// BUG: Doens't fully work entity lags behind camera rotation
			player.getComponent<CameraComponent>().camera->addChild(child_entity.getComponent<TransformComponent>().node);
			child_entity.getComponent<TransformComponent>().isChild = true;
			
		}
		else {
			throw ex_ent_invalid_comp;
		}
	}
	catch (std::exception& ex)
	{
		spdlog::error(std::string(ex.what()) + "\'" + std::to_string(child) + "\'" + " Failed to set camera to parent in: ScriptBindings::AS_Set_Camera_To_Entity");
	}
}

static entityid AS_Spawn_Entity_0(std::string e)
{
	// DEBUG: Do not use '_asset_ent' macro here as it converts the string 'e' to a string, causing AS to not call the function
	return WorldManager::Get()->spawnEntity(g_entity_path + e + std::string(".ent"));
}

static entityid AS_Spawn_Entity_1(std::string e, irr::core::vector3df p)
{
	// DEBUG: Do not use '_asset_ent' macro here as it converts the string 'e' to a string, causing AS to not call the function
    return WorldManager::Get()->spawnEntity(g_entity_path + e + std::string(".ent"), "", false, p);
}

static entityid AS_Spawn_Entity_2(std::string e, irr::core::vector3df p, irr::core::vector3df r)
{
	// DEBUG: Do not use '_asset_ent' macro here as it converts the string 'e' to a string, causing AS to not call the function
    return WorldManager::Get()->spawnEntity(g_entity_path + e + std::string(".ent"), "", false, p, r);
}

static entityid AS_Spawn_Entity_3(std::string e, irr::core::vector3df p, irr::core::vector3df r, irr::core::vector3df s)
{
	// DEBUG: Do not use '_asset_ent' macro here as it converts the string 'e' to a string, causing AS to not call the function
    return WorldManager::Get()->spawnEntity(g_entity_path + e + std::string(".ent"), "", false, p, r, s);
}

static void AS_Kill_Entity_By_ID(entityid e)
{
    WorldManager::Get()->killEntityByID(e);
}

static void AS_Kill_Entity_By_Name(std::string& name)
{
    WorldManager::Get()->killEntityByName(name);
}

void ScriptBindings::RegisterEntity(asIScriptEngine* engine)
{
    engine->SetDefaultNamespace("transform");
    {
        engine->RegisterGlobalFunction("vector3d position(int)", asFUNCTION(AS_Get_Entity_Position), asCALL_CDECL);
        engine->RegisterGlobalFunction("void position(int, vector3d)", asFUNCTION(AS_Set_Entity_Position), asCALL_CDECL);
        engine->RegisterGlobalFunction("vector3d rotation(int)", asFUNCTION(AS_Get_Entity_Rotation), asCALL_CDECL);
        engine->RegisterGlobalFunction("void rotation(int, vector3d)", asFUNCTION(AS_Set_Entity_Rotation), asCALL_CDECL);
        engine->RegisterGlobalFunction("vector3d scale(int)", asFUNCTION(AS_Get_Entity_Scale), asCALL_CDECL);
        engine->RegisterGlobalFunction("void scale(int, vector3d)", asFUNCTION(AS_Set_Entity_Scale), asCALL_CDECL);

        engine->RegisterGlobalFunction("void move(int, vector3d)", asFUNCTION(AS_Move_Entity), asCALL_CDECL);
        engine->RegisterGlobalFunction("void rotate(int, vector3d)", asFUNCTION(AS_Rotate_Entity), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("entity");
    {
        engine->RegisterGlobalFunction("int spawn(string)", asFUNCTION(AS_Spawn_Entity_0), asCALL_CDECL);
        engine->RegisterGlobalFunction("int spawn(string, vector3d)", asFUNCTION(AS_Spawn_Entity_1), asCALL_CDECL);
        engine->RegisterGlobalFunction("int spawn(string, vector3d, vector3d)", asFUNCTION(AS_Spawn_Entity_2), asCALL_CDECL);
        engine->RegisterGlobalFunction("int spawn(string, vector3d, vector3d, vector3d)", asFUNCTION(AS_Spawn_Entity_3), asCALL_CDECL);

        engine->RegisterGlobalFunction("void kill(int)", asFUNCTION(AS_Kill_Entity_By_ID), asCALL_CDECL);
        engine->RegisterGlobalFunction("void kill(string)", asFUNCTION(AS_Kill_Entity_By_Name), asCALL_CDECL);

        engine->RegisterGlobalFunction("string get(int)", asFUNCTION(AS_Get_Entity_Name_By_ID), asCALL_CDECL);
        engine->RegisterGlobalFunction("int get(string)", asFUNCTION(AS_Get_Entity_ID_By_Name), asCALL_CDECL);

        engine->RegisterGlobalFunction("void setParent(int, int)", asFUNCTION(AS_Set_Entity_Parent), asCALL_CDECL);
        engine->RegisterGlobalFunction("void setParentCamera(int)", asFUNCTION(AS_Set_Camera_To_Entity), asCALL_CDECL);
    }

    engine->SetDefaultNamespace("");
}