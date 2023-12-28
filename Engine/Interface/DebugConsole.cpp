#include "Engine/Interface/DebugConsole.h"

#include "Engine/Resource/FilePaths.h"

#include <algorithm>

#include <IMGUI/imgui.h>

#include "Engine/Engine.h"
#include "Editor/Interface/EditorInterface.h"

#include "Game/Components.h"

#define cmp_cmd(x) cmdlet.at(0) == x
#define _arga cmdlet.at(1)
#define _argb cmdlet.at(2)

void SetIMGUI_DebugConsoleTheme()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.898f, 0.850f, 0.858f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.498f, 0.450f, 0.458f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.6f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.5f, 0.5f, 0.5f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.5f, 0.5f, 0.5f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.6f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.84f, 0.78f, 0.78f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.45f, 0.72f, 0.72f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.847f, 0.780f, 0.650f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_TooltipBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void DebugConsole::clearInputBuffer() { std::fill(m_inputBuffer, m_inputBuffer + sizeof m_inputBuffer, '\0'); }

void DebugConsole::draw(double dt)
{
    SetIMGUI_DebugConsoleTheme();

    static std::string selectedEntityName = std::string();

    static bool mouseClick = false;
    if (InputManager::Get()->getMousePressOnce(0, &mouseClick, true) && InputManager::Get()->
        isKeyPressed(KEYBOARD_KEY::KEY_LSHIFT, true))
    {
        auto node = RenderManager::Get()->getNodeFromCursorPosition();
        if (node)
        {
            m_selectedEntity = node->getID();
            selectedEntityName = WorldManager::Get()->managerSystem()->getNameByID(m_selectedEntity);
        }
    }

    /*ImGui::SetNextWindowSize(ImVec2(480, 64));
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(RenderManager::Get()->getConfiguration().width) / 2 - 240, 4));
    if (ImGui::Begin("EntitySelectedHelper", reinterpret_cast<bool*>(1),
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        ImGui::Text(std::string("FPS: " + std::to_string(RenderManager::Get()->device()->getVideoDriver()->getFPS())).c_str());
        ImGui::End();
    }*/

    ImGui::SetNextWindowSize(ImVec2(500.0f, static_cast<float>(RenderManager::Get()->getConfiguration().height)));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if (ImGui::Begin("Console", nullptr,
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        if (ImGui::InputText("", m_inputBuffer, static_cast<int>(sizeof m_inputBuffer / sizeof *m_inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (m_inputBuffer[0] != '`' && m_inputBuffer[0] != '\0' &&m_inputBuffer[0] != ' ' && m_inputBuffer[0] != '\t' && m_inputBuffer[0] != '~')
            {
                m_stringDisplayList.emplace_back("> " + std::string(m_inputBuffer));
                executecmd(std::string(m_inputBuffer));
                clearInputBuffer();
            }
        }

        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

            std::for_each(m_stringDisplayList.rbegin(), m_stringDisplayList.rend(),
                          [](std::string& elem) { ImGui::TextUnformatted(elem.c_str()); });

            ImGui::PopStyleVar();

            if (m_scrollToBottom)
            {
                //ImGui::SetScrollHere();
            }

            ImGui::EndChild();
        }

        if (ImGui::IsItemHovered() || ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
        {
			if (!entityProps())
			{
				ImGui::SetKeyboardFocusHere(-1);
			}
        }

        ImGui::End();
    }

	static bool repos = true;
    if (entityProps()) 
	{
		if (repos)
		{
			ImGui::SetNextWindowPos(ImVec2(static_cast<float>(RenderManager::Get()->getConfiguration().width) - 500.0f, 0.0f));
			repos = false;
		}
    	
		EditorInterface::draw_window_prop_ent(true);

		ImGui::GetIO().MouseDrawCursor = true;
    }
	else
	{
		ImGui::GetIO().MouseDrawCursor = false;
		repos = true;
	}

	// Scene Hierarchy
	if (m_drawHiearch)
	{
		ImGui::SetNextWindowSize(ImVec2(200.0f, static_cast<float>(RenderManager::Get()->getConfiguration().height)));
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(RenderManager::Get()->getConfiguration().width) - 200.0f, 0.0f));
		if (ImGui::Begin("SceneHierarchy__", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetNextTreeNodeOpened(true);
			// NOIMP It works but doesn't show parent/child relationships, not quite a real hierarchy lol
			ImGui::CollapsingHeader("Scene Hierarchy");

			// TODO: Very slow way to iterate over the world
			// BUG: Entities of the same name will not be selectable as IMGUI uses names internally as ID's
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.6f, 0.0f));
			for (auto ent : WorldManager::Get()->world()->getEntities())
			{
				ImGui::PushID(std::string(ent.getComponent<DescriptorComponent>().name + std::to_string(ent.getComponent<DescriptorComponent>().id)).c_str());
				if (ImGui::Button(ent.getComponent<DescriptorComponent>().name.c_str(), ImVec2(180, 16)))
				{
					m_selectedEntity = ent.getComponent<DescriptorComponent>().id;
					selectedEntityName = ent.getComponent<DescriptorComponent>().name;

					g_currentSelectedObjectType = 1;
					g_currentEntity = m_selectedEntity;
				}
				ImGui::PopID();
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}

    ImGui::PopStyleColor(16);
}

void DebugConsole::draw_stats()
{
    if (Engine::Get()->isDefaultStatsDrawingEnabled())
    {
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		{
#ifdef NDEBUG
			ImGui::Text("RELEASE MODE\n\nFPS : %i\nDEL : %i\n\nTRI : %i\n\nSCN : %i\nENT : %i\nPHY : %i",
#else
			ImGui::Text("FPS : %i\nDEL : %i\n\nTRI : %i\n\nSCN : %i\nENT : %i\nPHY : %i",
#endif
				RenderManager::Get()->driver()->getFPS() + 1,
				static_cast<int>(Engine::Get()->getDeltaTime()) - 1,
				RenderManager::Get()->driver()->getPrimitiveCountDrawn(),
				RenderManager::Get()->sceneManager()->getRootSceneNode()->getChildren().size(),
				WorldManager::Get()->world()->getEntityCount(),
				PhysicsManager::Get()->scene()->getNbActors(physx::PxActorTypeFlags(physx::PxActorTypeFlag::eRIGID_DYNAMIC)));

			ImGui::End();
		}
    }
}

// Depends on the player being named 'player'
void DebugConsole::drawPlayerInfo()
{
    if (m_drawPlayer)
    {
		if (!Engine::Get()->isDebugConsoleVisible())
		{
			auto windowWidth = 320, windowHeight = 320;
			ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
			ImGui::SetNextWindowPos(ImVec2(0, 0));

			auto& ent_player = WorldManager::Get()->managerSystem()->getEntityByName("player");
			auto& transform = ent_player.getComponent<TransformComponent>();
			auto& camera = ent_player.getComponent<CameraComponent>();

			if (ImGui::Begin("PlayerInfo", reinterpret_cast<bool*>(1),
				ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize))
			{
				ImGui::Text("EPX %f", transform.position.X);
				ImGui::Text("EPY %f", transform.position.Y);
				ImGui::Text("EPZ %f", transform.position.Z);
				ImGui::Text("ERX %f", transform.rotation.X);
				ImGui::Text("ERY %f", transform.rotation.Y);
				ImGui::Text("ERZ %f", transform.rotation.Z);
				ImGui::Text("CPX %f", camera.camera->getPosition().X);
				ImGui::Text("CPY %f", camera.camera->getPosition().Y);
				ImGui::Text("CPZ %f", camera.camera->getPosition().Z);
				ImGui::Text("CRX %f", camera.camera->getRotation().X);
				ImGui::Text("CRY %f", camera.camera->getRotation().Y);
				ImGui::Text("CRZ %f", camera.camera->getRotation().Z);
				ImGui::Text("CNX %f", camera.lookat.X);
				ImGui::Text("CNY %f", camera.lookat.Y);
				ImGui::Text("CNZ %f", camera.lookat.Z);

				ImGui::End();
			}
		}
    }
}

void DebugConsole::executecmd(const std::string& cmd)
{
    std::istringstream buffer(cmd);
    std::vector<std::string> cmdlet((std::istream_iterator<std::string>(buffer)),
                                    std::istream_iterator<std::string>());

    bool
        arga_trigger = cmdlet.size() >= 2,
        argb_trigger = cmdlet.size() == 3;

#define _arga_exists arga_trigger
#define _argb_exists argb_trigger

    if (cmdlet.empty()) { return; }

    if (cmp_cmd("clear") || cmp_cmd("cls"))
    {
        m_stringDisplayList.clear();
        return;
    }

    if (cmp_cmd("echo"))
    {
        if (_arga_exists)
        {
            log(_arga);
            return;
        }
        return;
    }

	if (cmp_cmd("debug"))
	{
		Engine::Get()->setGameDebugFeaturesEnabled(!Engine::Get()->isGameDebugFeaturesEnabled());
		return;
	}

    if (cmp_cmd("stats"))
    {
		Engine::Get()->setDefaultStatsDrawingEnabled(!Engine::Get()->isDefaultStatsDrawingEnabled());
        return;
    }

    if (cmp_cmd("scene"))
    {
        if (_arga_exists)
        {
            //WorldManager::Get()->killAllEntities();
            //WorldManager::Get()->importScene(_asset_scn(_arga));
            Engine::Get()->stateManager()->destroyState(ENGINE_STATE_ID::ESID_GAME);
            Engine::Get()->stateManager()->initState(ENGINE_STATE_ID::ESID_GAME, _asset_scn(_arga));
            return;
        }

        logerr_para("scene");
        return;
    }

    if (cmp_cmd("playerinfo")) { togglePlayerInfo(); return; }

    if (cmp_cmd("spawn"))
    {
        if (_arga_exists) {
                   auto rdata = PhysicsManager::Get()->raycast(
                       RenderManager::Get()->sceneManager()->getActiveCamera()->getAbsolutePosition(),
                       Math::GetDirectionVector(RenderManager::Get()->sceneManager()->getActiveCamera()->getRotation(), true),
                       100.0);
       
                   irr::core::vector3df target;
       
                   if (rdata.hit) {
                       target = Utility::PxVec3_To_IrrVec3(rdata.data.getAnyHit(0).position);
                   }
       
                   if (!_argb_exists) {
                       if (!WorldManager::Get()->spawnEntity(_asset_ent(_arga), "", false, target)) {
                           logerr("Entity \'" + _arga + "\' not found");
                       }
                   }
                   else {
                       for (auto i = 0U; i < static_cast<unsigned int>(atoi(_argb.c_str())); i++) {
                           if (!WorldManager::Get()->spawnEntity(_asset_ent(_arga), "", false, target)) {
                               logerr("Entity \'" + _arga + "\' not found");
                               return;
                           }
                       }
       
                       return;
                   }
       
                   return;
               }

        logerr_para("entity");
        return;
    }

    if (cmp_cmd("damage"))
    {
        /*if (_arga_exists) {
                   if (_argb_exists) {
                       if (WorldManager::Get()->managerSystem()->getEntityByName(_arga).isValid()) {
                           auto& entity = WorldManager::Get()->managerSystem()->getEntityByName(_arga);
                           if (entity.hasComponent<DamageReceiverComponent>()) {
                               WorldManager::Get()->damageSystem()->damageEntity(entity.getComponent<DescriptorComponent>().id, atoi(_argb.c_str()));
                               log("Damaged entity \'" + _arga + "\'");
                               return;
                           }
       
                           log("Entity \'" + _arga + "\' is unable to receive damage");
                           return;
                       }
                       logerr("Entity \'" + _arga + "\' is not valid");
                       return;
                   }
       
                   logerr_para("damage");
                   return;
               }*/

        logerr_para("entity");
        return;
    }

    if (cmp_cmd("heal"))
    {
        /*if (_arga_exists) {
                   if (_argb_exists) {
                       if (WorldManager::Get()->getEntityByName(_arga).isValid()) {
                           auto& entity = WorldManager::Get()->getEntityByName(_arga);
                           if (entity.hasComponent<DamageReceiverComponent>()) {
                               WorldManager::Get()->damageSystem()->healEntity(entity.getComponent<DescriptorComponent>().id, atoi(_argb.c_str()));
                               log("Healed entity \'" + _arga + "\'");
                               return;
                           }
       
                           log("Entity \'" + _arga + "\' is unable to receive healing");
                           return;
                       }
                       logerr("Entity \'" + _arga + "\' is not valid");
                       return;
                   }
       
                   logerr_para("heal");
                   return;
               }*/

        logerr_para("entity");
        return;
    }

    if (cmp_cmd("kill") && !_arga_exists)
    {
        if (m_selectedEntity < _entity_null_value)
        {
            auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(m_selectedEntity);

            if (entity.isValid())
            {
                if (entity.getComponent<DescriptorComponent>().type == ET_DYNAMIC)
                {
                    if (!WorldManager::Get()->killEntityByID(m_selectedEntity))
                    {
                        logerr("Entity \'" + std::to_string(m_selectedEntity) + "\' not found");
                    }
                    else { m_selectedEntity = _entity_null_value; }
                }
                else { logerr("Entity \'" + std::to_string(m_selectedEntity) + "\' lacks type ET_DYNAMIC"); }
            }
            else { logerr("Entity \'" + std::to_string(m_selectedEntity) + "\' not found"); }
        }

        return;
    }

    if (cmp_cmd("prop") || cmp_cmd("props") || cmp_cmd("properties"))
    {
        toggleEntityProps();
        return;
    }

	if (cmp_cmd("scenelist"))
	{
		toggleHiearch();
		return;
	}

	if (cmp_cmd("debugvis"))
	{
		WorldManager::Get()->renderSystem()->setDebugSpriteVisible(
			!WorldManager::Get()->renderSystem()->isDebugSpriteVisible());
		return;
	}

    if (cmp_cmd("cvar"))
    {
        if (_arga_exists)
        {
            if (WorldManager::Get()->getCVarExists(_arga))
            {
                log("CVar \'" + _arga + "\' contains value: " + WorldManager::Get()->getCVarValue(_arga));
                return;
            }
            logerr("CVar \'" + _arga + "\' does not exist");
            return;
        }
        if (/*_arga_exists &&*/ _argb_exists)
        {
            if (WorldManager::Get()->getCVarExists(_arga))
            {
                WorldManager::Get()->setCVar(_arga, _argb);
                log("Set CVar \'" + _arga + "\' to value: " + _argb);
                return;
            }

            WorldManager::Get()->setCVar(_arga, _argb);
            log("Created CVar \'" + _arga + "\' with value: " + _argb);
            return;
        }

        logerr_para("CVar\\Value");
        return;
    }

    logerr(std::string(_arga_exists ? "Unknown input\\argument(s)" : "Unknown input") + " \'" + cmd + "\'");
}

int DebugConsole::argtobool(const std::string& arg)
{
    if (arg == "0" || arg == "false") { return 0; }
    if (arg == "1" || arg == "true") { return 1; }

    logerr("Invalid argument, expected boolean");

    return -1;
}

void DebugConsole::log(const std::string& str) { m_stringDisplayList.push_back(str); }
void DebugConsole::logwarn(const std::string& str) { m_stringDisplayList.push_back("WARNING: " + str); }
void DebugConsole::logerr(const std::string& str) { m_stringDisplayList.push_back("ERROR: " + str); }

void DebugConsole::logerr_para(const std::string& str)
{
    m_stringDisplayList.push_back("ERROR: Missing\\Invalid parameter \'" + str + "\'");
}
