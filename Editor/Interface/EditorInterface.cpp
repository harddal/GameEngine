#include "EditorInterface.h"

#include "Editor/EditorState.h"

#include "Engine/Resource/FilePaths.h"

#include "Utility/Utility.h"

#include <irrimgui/includes/IrrIMGUI/IrrIMGUI.h>

#include "Engine/Interface/ImGuiExtensions.h"

#include "Shlwapi.h"
#include <iomanip>
#include "Editor/EditorState.h"
#include <boost/range/iterator_range.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>

#include "Engine/Engine.h"

#include "Game/Components.h"

using namespace boost;
using namespace filesystem;

std::vector<entityid> g_undoEntities;

std::vector<std::string> m_entityList, m_textureList, m_prefabList, m_meshList;
std::vector<irr::video::ITexture*> m_imageEntityIconList;
std::vector<IrrIMGUI::IGUITexture*> m_imguiEntityIconList;
std::vector<irr::video::ITexture*> m_imageTextureFileList;
std::vector<IrrIMGUI::IGUITexture*> m_imguiTextureList;

EditorWindowData m_windowData;

bool m_hasLoadedEntityList = false, m_hasLoadedTextureList = false, m_hasLoadedPrefabList = false, m_hasLoadedMeshList =
	     false;

void SetIMGUI_SceneEditorTheme()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.898f, 0.850f, 0.858f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.498f, 0.450f, 0.458f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.6f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.6f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.84f, 0.78f, 0.78f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.45f, 0.72f, 0.72f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.847f, 0.780f, 0.650f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_TooltipBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void EditorInterface::draw()
{
	SetIMGUI_SceneEditorTheme();

	detectKeyShortcuts();

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
	ImGui::Begin("editor_main_window", nullptr,
	             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
	             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		ImGui::PopStyleColor();

		draw_menubar_main();

		draw_window_spawn_entity();
		draw_window_spawn_prefab();
		draw_window_spawn_mesh();
		draw_window_hierarchy();
		draw_window_prop_ent();
		draw_window_prop_scene();
		draw_window_help_about();
		draw_window_scene_stats();
		draw_window_console();
		draw_window_log();
		draw_window_editor_settings();
		draw_window_texture_browser();
		draw_window_add_component();
		draw_window_entity_debug_info();

		g_sceneInteractor.draw();
	}
	ImGui::End();

	ImGui::PopStyleColor(16);
}

void EditorInterface::function_open_scene()
{
	std::string path = Utility::OpenFileDialog(dialog_filter_scene);

	if (PathFileExistsA(path.c_str()))
	{
		WorldManager::Get()->killAllEntities();
		WorldManager::Get()->importScene(path);
	}
}

void EditorInterface::funtion_save_scene()
{
	SceneDescriptor scenedesc;
	scenedesc.ambient_light = RenderManager::Get()->sceneManager()->getAmbientLight();
	scenedesc.skydome_texture = RenderManager::Get()->getCurrentSkydomeTexture();
	scenedesc.name = "null";
	WorldManager::Get()->exportScene(Utility::SaveFileDialog(dialog_filter_scene)); /*, scenedesc);*/
}

void EditorInterface::function_play_scene()
{
	Engine::Get()->stateManager()->setStatePauseResume(ESID_EDITORGAME);
}

void EditorInterface::function_showhide_menubar()
{
	m_windowData.draw_menubar_main = !m_windowData.draw_menubar_main;
}

void EditorInterface::draw_menubar_main()
{
	if (!m_windowData.draw_menubar_main) { return; }

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				const int result = MessageBox(nullptr, "Destroy current scene?", "New Scene", MB_YESNO);
				switch (result)
				{
				case IDYES: WorldManager::Get()->killAllEntities();
					break;
				default: break;
				}
			}
			if (ImGui::MenuItem("Open", "CTRL-O"))
			{
				function_open_scene();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				funtion_save_scene();
			}
			if (ImGui::MenuItem("Save As..."))
			{
				funtion_save_scene();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit", "ALT+F4"))
			{
				Engine::Get()->exit();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, false))
			{
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
			{
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "CTRL+X")) { g_sceneInteractor.cutEntity(); }
			if (ImGui::MenuItem("Copy", "CTRL+C")) { g_sceneInteractor.copyEntity(); }
			if (ImGui::MenuItem("Paste", "CTRL+V")) { g_sceneInteractor.pasteEntity(); }
			if (ImGui::MenuItem("Delete", "DEL")) { g_sceneInteractor.deleteEntity(); }

			ImGui::Separator();

			if (ImGui::MenuItem("Select All", "CTRL+A", false, false))
			{
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Show/Hide Menubar", "CTRL+M"))
			{
				function_showhide_menubar();
			}


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Project")) { ImGui::EndMenu(); }

		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Link Entities", "CTRL+L"))
			{
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Game"))
		{
			if (ImGui::MenuItem("Play", "CTRL+G"))
			{
				function_play_scene();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Scene Hierarchy", "CTRL+H")) { m_windowData.draw_window_hiearchy = true; }
			if (ImGui::MenuItem("Scene Properties", "")) { m_windowData.draw_window_prop_scene = true; }
			if (ImGui::MenuItem("Properties Menu", "CTRL+P")) { m_windowData.draw_window_prop_ent = true; }
			if (ImGui::MenuItem("Entity Spawn Menu", "CTRL+E")) { m_windowData.draw_window_spawn_entity = true; }
			if (ImGui::MenuItem("Prefab Spawn Menu", "CTRL+R")) { m_windowData.draw_window_spawn_prefab = true; }
			if (ImGui::MenuItem("Mesh Spawn Menu", "")) { m_windowData.draw_window_spawn_mesh = true; }
			if (ImGui::MenuItem("Texture Browser", "CTRL+T")) { draw_window_texture_browser(); }

			ImGui::Separator();

			if (ImGui::MenuItem("Console", "CTRL+TAB")) { m_windowData.draw_window_console = true; }

			if (ImGui::MenuItem("Log")) { m_windowData.draw_window_log = true; }

			ImGui::Separator();

			if (ImGui::MenuItem("Export Script Functions")) 
			{ 
				std::ofstream file(Utility::SaveFileDialog("Text Files\0*.txt\0Any File\0*.*\0"));

				for (auto i = 0U; i < ScriptManager::Get()->getEngine()->GetGlobalFunctionCount(); i++)
				{
					auto func = ScriptManager::Get()->getEngine()->GetGlobalFunctionByIndex(i);
					
					file << func->GetDeclaration(true, true, true) << ";\n";
				}

				file.close();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Scene Statistics", "")) { m_windowData.draw_window_scene_stats = true; }
			if (ImGui::MenuItem("Entity Debug Info", "")) { m_windowData.draw_window_entity_debug_info = true; }

			ImGui::Separator();

			//if (ImGui::BeginMenu("Settings"))
			//{
				if (ImGui::MenuItem("Editor Settings")) { m_windowData.draw_window_editor_settings = true; }

				//ImGui::EndMenu();
			//}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Widget"))
		{
			ImGui::Text("Mode: %s", g_sceneInteractor.getWidgetToolModeStr().c_str());
			if (ImGui::MenuItem("Translate", "CTRL+1"))
			{
				g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::TRANSLATE);
			}
			if (ImGui::MenuItem("Rotate", "CTRL+2"))
			{
				g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::ROTATE);
			}
			if (ImGui::MenuItem("Scale", "CTRL+3"))
			{
				g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::SCALE);
			}

			ImGui::Separator();

			ImGui::Text("Mode: %s", g_sceneInteractor.getWidgetCoordModeStr().c_str());
			if (ImGui::MenuItem("Local", "CTRL+4"))
			{
				g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::LOCAL);
			}
			if (ImGui::MenuItem("World", "CTRL+5"))
			{
				g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::WORLD);
			}

			ImGui::Separator();

			ImGui::Text(std::string(std::string("Snap: ") + (g_sceneInteractor.isSnap() ? "On" : "Off")).c_str());
			if (ImGui::MenuItem("Snap to Grid", "CTRL+6")) { g_sceneInteractor.useSnap(!g_sceneInteractor.isSnap()); }
			auto snap = g_sceneInteractor.getSnapUnit();
			if (ImGui::InputFloat("Unit", &snap)) { g_sceneInteractor.setSnap(snap); }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) { ImGui::EndMenu(); }

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About", "")) { m_windowData.draw_window_help_about = true; }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorInterface::detectKeyShortcuts()
{
	if (InputManager::Get()->isKeyPressed(KEYBOARD_KEY::KEY_DELETE))
	{
		g_sceneInteractor.deleteEntity();
	}

	auto control = false;
	if (InputManager::Get()->isKeyPressed(KEYBOARD_KEY::KEY_LCONTROL))
	{
		control = true;
	}

	{
		if (control && InputManager::Get()->isKeyPressed(KEY_O))
		{
			function_open_scene();
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_S))
		{
			funtion_save_scene();
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_P))
		{
			m_windowData.draw_window_prop_ent = true;
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_H))
		{
			m_windowData.draw_window_hiearchy = true;
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_G))
		{
			function_play_scene();
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_M))
		{
			function_showhide_menubar();
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_E))
		{
			m_windowData.draw_window_spawn_entity = true;
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_R))
		{
			m_windowData.draw_window_spawn_prefab = true;
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_L))
		{
			/*LINK ENTITIES*/
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_TAB))
		{
			m_windowData.draw_window_console = true;
		}
	}
	{
		if (control && InputManager::Get()->isKeyPressed(KEY_T))
		{
			draw_window_texture_browser();
		}
	}
	{
		static bool key_x = false;
		if (control && InputManager::Get()->getKeyRelease(KEY_X, &key_x))
		{
			g_sceneInteractor.cutEntity();
		}

		static bool key_c = false;
		if (control && InputManager::Get()->getKeyRelease(KEY_C, &key_c))
		{
			g_sceneInteractor.copyEntity();
		}

		static bool key_v = false;
		if (control && InputManager::Get()->getKeyRelease(KEY_V, &key_v))
		{
			g_sceneInteractor.pasteEntity();
		}

		static bool key_z = false;
		if (control && InputManager::Get()->getKeyRelease(KEY_Z, &key_z))
		{
			if (g_undoEntities.size())
			{
				WorldManager::Get()->killEntityByID(g_undoEntities.back());

				g_sceneInteractor.clearSelectedEntities();

				WorldManager::Get()->spawnEntity("undo.prev" + std::to_string(g_undoEntities.size()), "", true);

				g_undoEntities.pop_back();
			}
		}
	}

	{
		if (control && InputManager::Get()->isKeyPressed(KEY_NUM1))
		{
			g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::TRANSLATE);
		}
		if (control && InputManager::Get()->isKeyPressed(KEY_NUM2))
		{
			g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::ROTATE);
		}
		if (control && InputManager::Get()->isKeyPressed(KEY_NUM3))
		{
			g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::SCALE);
		}

		if (control && InputManager::Get()->isKeyPressed(KEY_NUM4))
		{
			g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::LOCAL);
		}
		if (control && InputManager::Get()->isKeyPressed(KEY_NUM5))
		{
			g_sceneInteractor.setTransformWidgetMode(TRANSFORM_WIDGET_MODE::WORLD);
		}
		if (control && InputManager::Get()->isKeyPressed(KEY_NUM6))
		{
			g_sceneInteractor.useSnap(!g_sceneInteractor.isSnap());
		}
	}
}

void EditorInterface::draw_window_hierarchy()
{
	if (!m_windowData.draw_window_hiearchy) { return; }

	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(250, 600));
	if (ImGui::Begin("Scene Hierarchy", &m_windowData.draw_window_hiearchy, ImGuiWindowFlags_AlwaysAutoResize))
	{
		for (auto ent : WorldManager::Get()->world()->getEntities())
		{
			ImGui::PushID(std::string(ent.getComponent<DescriptorComponent>().name + std::to_string(ent.getComponent<DescriptorComponent>().id)).c_str());
			if (ImGui::TreeNode(ent.getComponent<DescriptorComponent>().name.c_str()))
			{
				ImGui::Text(std::string("Name: " + ent.getComponent<DescriptorComponent>().name).c_str());
				ImGui::Text("ID:   %d", ent.getComponent<DescriptorComponent>().id);

				ImGui::Spacing();

				if (ImGui::TreeNode("Transform##extra"))
				{
					auto transform = ent.getComponent<TransformComponent>();

					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << transform.position.X << ", " << transform.position.Y <<
						", " << transform.position.Z << "";
					ImGui::Text(std::string("P: " + ss.str()).c_str());
					ss.str(std::string());
					ss << std::fixed << std::setprecision(2) << transform.rotation.X << ", " << transform.rotation.Y <<
						", " << transform.rotation.Z << "";
					ImGui::Text(std::string("R: " + ss.str()).c_str());
					ss.str(std::string());
					ss << std::fixed << std::setprecision(2) << transform.scale.X << ", " << transform.scale.Y << ", "
						<< transform.scale.Z << "";
					ImGui::Text(std::string("S: " + ss.str()).c_str());

					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void EditorInterface::draw_window_prop_ent(bool display_override)
{
	if (display_override) { m_windowData.draw_window_prop_ent = true; }
	
	if (!m_windowData.draw_window_prop_ent) { return; }

	std::string label;

	switch (static_cast<SELECTED_OBJECT_TYPE>(g_currentSelectedObjectType))
	{
	case SELECTED_OBJECT_TYPE::NONE:
		label = "Properties";
		break;
	case SELECTED_OBJECT_TYPE::ENTITY:
		label = "Entity Properties";
		break;
	case SELECTED_OBJECT_TYPE::MESH:
		label = "Mesh Properties";
		break;
	case SELECTED_OBJECT_TYPE::PREFAB:
		label = "Prefab Properties";
		break;
	default:
		label = "Properties";
		break;
	}

	if (ImGui::Begin(label.c_str(), &m_windowData.draw_window_prop_ent, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// MESH
		/*if (g_currentSelectedObjectType == static_cast<unsigned int>(SELECTED_OBJECT_TYPE::MESH) && g_currentMesh < _entity_null_value) {
		    auto mesh = WorldManager::Get()->getStaticMeshNode(g_currentMesh);

		    float fl3_pos[3];
		    mesh->getPosition().getAs3Values(fl3_pos);
		    if (ImGui::InputFloat3("Positon", fl3_pos, 2, ImGuiInputTextFlags_EnterReturnsTrue)) {
		        mesh->setPosition(irr::core::vector3df(fl3_pos[0], fl3_pos[1], fl3_pos[2]));
		        WorldManager::Get()->renderSystem()->forceTransformUpdate();
		    }
		    float fl3_rot[3];
		    mesh->getRotation().getAs3Values(fl3_rot);
		    if (ImGui::InputFloat3("Rotation", fl3_rot, 2, ImGuiInputTextFlags_EnterReturnsTrue)) {
		        mesh->setRotation(irr::core::vector3df(fl3_rot[0], fl3_rot[1], fl3_rot[2]));
		        WorldManager::Get()->renderSystem()->forceTransformUpdate();
		    }
		    float fl3_scl[3];
		    mesh->getScale().getAs3Values(fl3_scl);
		    if (ImGui::InputFloat3("Scale", fl3_scl, 2, ImGuiInputTextFlags_EnterReturnsTrue)) {
		        mesh->setScale(irr::core::vector3df(fl3_scl[0], fl3_scl[1], fl3_scl[2]));
		        WorldManager::Get()->renderSystem()->forceTransformUpdate();
		    }
		}*/

		// ENTITY
		if (g_currentSelectedObjectType == static_cast<unsigned int>(SELECTED_OBJECT_TYPE::ENTITY) && g_currentEntity <
			_entity_null_value)
		{
			auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(g_currentEntity);

			if (entity.isValid())
			{
				ImGui::SetNextTreeNodeOpened(true);
				if (entity.hasComponent<DescriptorComponent>() && ImGui::CollapsingHeader("Descriptor"))
				{
					draw_component_properties(ENTITY_COMPONENT::DESCRIPTOR, entity);
				}

				ImGui::SetNextTreeNodeOpened(true);
				if (entity.hasComponent<TransformComponent>() && ImGui::CollapsingHeader("Transform"))
				{
					draw_component_properties(ENTITY_COMPONENT::TRANSFORM, entity);
				}

				if (entity.hasComponent<DataComponent>() && ImGui::CollapsingHeader("Data"))
				{
					draw_component_properties(ENTITY_COMPONENT::DATA, entity);
				}

				if (entity.hasComponent<RenderComponent>() && ImGui::CollapsingHeader("Render"))
				{
					draw_component_properties(ENTITY_COMPONENT::RENDER, entity);
				}

				if (entity.hasComponent<MeshComponent>() && ImGui::CollapsingHeader("Mesh"))
				{
					draw_component_properties(ENTITY_COMPONENT::MESH, entity);
				}

				if (entity.hasComponent<CameraComponent>() && ImGui::CollapsingHeader("Camera"))
				{
					draw_component_properties(ENTITY_COMPONENT::CAMERA, entity);
				}

				if (entity.hasComponent<LightComponent>() && ImGui::CollapsingHeader("Light"))
				{
					draw_component_properties(ENTITY_COMPONENT::LIGHT, entity);
				}

				if (entity.hasComponent<PhysicsComponent>() && ImGui::CollapsingHeader("Physics"))
				{
					draw_component_properties(ENTITY_COMPONENT::PHYSICS, entity);
				}

				if (entity.hasComponent<CCTComponent>() && ImGui::CollapsingHeader("CCT"))
				{
					draw_component_properties(ENTITY_COMPONENT::CHARACTERCONTROLLER, entity);
				}

				if (entity.hasComponent<BillboardSpriteComponent>() && ImGui::CollapsingHeader("Billboard Sprite"))
				{
					draw_component_properties(ENTITY_COMPONENT::BILLBOARDSPRITE, entity);
				}

				if (entity.hasComponent<DebugSpriteComponent>() && ImGui::CollapsingHeader("DebugSprite"))
				{
					draw_component_properties(ENTITY_COMPONENT::DEBUGSPRITE, entity);
				}

				if (entity.hasComponent<DebugMeshComponent>() && ImGui::CollapsingHeader("DebugMesh"))
				{
					draw_component_properties(ENTITY_COMPONENT::DEBUGMESH, entity);
				}

				if (entity.hasComponent<ScriptComponent>() && ImGui::CollapsingHeader("Script"))
				{
					draw_component_properties(ENTITY_COMPONENT::SCRIPT, entity);
				}

				if (entity.hasComponent<LogicComponent>() && ImGui::CollapsingHeader("Logic Event"))
				{
					draw_component_properties(ENTITY_COMPONENT::LOGIC, entity);
				}

				if (entity.hasComponent<SoundComponent>() && ImGui::CollapsingHeader("Sound"))
				{
					draw_component_properties(ENTITY_COMPONENT::SOUND, entity);
				}

				if (entity.hasComponent<MarkerComponent>() && ImGui::CollapsingHeader("Marker"))
				{
					draw_component_properties(ENTITY_COMPONENT::MARKER, entity);
				}

				if (entity.hasComponent<TriggerZoneComponent>() && ImGui::CollapsingHeader("Trigger Zone"))
				{
					draw_component_properties(ENTITY_COMPONENT::TRIGGERZONE, entity);
				}

				if (entity.hasComponent<PrefabComponent>() && ImGui::CollapsingHeader("Prefab"))
				{
					draw_component_properties(ENTITY_COMPONENT::PREFAB, entity);
				}

				if (entity.hasComponent<NPCComponent>() && ImGui::CollapsingHeader("NPC"))
				{
					draw_component_properties(ENTITY_COMPONENT::NPC, entity);
				}

				if (entity.hasComponent<AutoKillComponent>() && ImGui::CollapsingHeader("Auto Kill"))
				{
					draw_component_properties(ENTITY_COMPONENT::AUTOKILL, entity);
				}

				if (entity.hasComponent<DamageReceiverComponent>() && ImGui::CollapsingHeader("Damage Receiver"))
				{
					draw_component_properties(ENTITY_COMPONENT::DAMAGERECEIVER, entity);
				}

				if (entity.hasComponent<ItemComponent>() && ImGui::CollapsingHeader("Item"))
				{
					draw_component_properties(ENTITY_COMPONENT::ITEM, entity);
				}

				if (entity.hasComponent<SoundListenerComponent>() && ImGui::CollapsingHeader("Sound Listener"))
				{
					draw_component_properties(ENTITY_COMPONENT::SOUNDLISTENER, entity);
				}

				ImGui::Separator();
				
				if (ImGui::Button("Add Component..."))
				{
					m_windowData.draw_window_add_component = true;
				}
			}
			else
			{
				ImGui::Text("No valid entity selected");
			}
		}
		else
		{
			ImGui::Text("No Valid Entity Selected!");
		}
	}
	ImGui::End();
}

void EditorInterface::draw_window_prop_scene()
{
	if (!m_windowData.draw_window_prop_scene) { return; }

	if (ImGui::Begin("Scene Properties", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static auto scenedesc = WorldManager::Get()->getCurrentSceneDescriptor();
		
		char buf_name[256];
		memset(buf_name, 0, 256);
		for (auto i = 0U; i < scenedesc.name.size() && i < 256; i++)
		{
			buf_name[i] = scenedesc.name[i];
		}
		ImGui::PushID("SceneNameInput");
		if (ImGui::InputText("", buf_name, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			scenedesc.name = buf_name;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Name");

		char buf_creator[256];
		memset(buf_creator, 0, 256);
		for (auto i = 0U; i < scenedesc.creator.size() && i < 256; i++)
		{
			buf_creator[i] = scenedesc.creator[i];
		}
		ImGui::PushID("SceneCreatorInput");
		if (ImGui::InputText("", buf_creator, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			scenedesc.creator = buf_creator;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Creator");

		char buf_notes[256];
		memset(buf_notes, 0, 256);
		for (auto i = 0U; i < scenedesc.notes.size() && i < 256; i++)
		{
			buf_notes[i] = scenedesc.notes[i];
		}
		ImGui::PushID("SceneNotesInput");
		if (ImGui::InputText("", buf_notes, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			scenedesc.notes = buf_notes;
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Text("Notes");
		
		if (ImGui::Button("Set Skydome Texture"))
		{
			auto skydome = Utility::OpenFileDialog(dialog_filter_image);

			//if (PathFileExistsA(skydome.c_str()))
			//{
			std::size_t found = skydome.find("content\\");

			if (found != std::string::npos)
			{
				auto skydome_path = skydome.substr(found);

				scenedesc.skydome_texture = skydome_path;
			}
			else
			{
				Utility::Warning("Assets must exist in local path!");
			}
			//}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Ambient Light:");
		ImColor color(scenedesc.ambient_light.r, scenedesc.ambient_light.g, scenedesc.ambient_light.b, scenedesc.ambient_light.a);
		ImColorPicker("", &color);
		float v[3];
		v[0] = color.Value.x;
		v[1] = color.Value.y;
		v[2] = color.Value.z;
		scenedesc.ambient_light = irr::video::SColorf(v[0], v[1], v[2]);
		RenderManager::Get()->sceneManager()->setAmbientLight(scenedesc.ambient_light);

		if (ImGui::Button("Save")) 
		{
			RenderManager::Get()->swapSkyDomeTexture(scenedesc.skydome_texture);
			WorldManager::Get()->setCurrentSceneDescriptor(scenedesc);

			scenedesc = WorldManager::Get()->getCurrentSceneDescriptor();

			m_windowData.draw_window_prop_scene = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			auto desc = WorldManager::Get()->getCurrentSceneDescriptor();
			WorldManager::Get()->setCurrentSceneDescriptor(desc);

			RenderManager::Get()->sceneManager()->setAmbientLight(desc.ambient_light);

			scenedesc = WorldManager::Get()->getCurrentSceneDescriptor();
			
			m_windowData.draw_window_prop_scene = false;
		}
	}
	ImGui::End();
}

void EditorInterface::draw_window_help_about()
{
	if (!m_windowData.draw_window_help_about) { return; }

	if (ImGui::Begin("About", &m_windowData.draw_window_help_about, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Engine v%s", "0.3a"/*ENGINE_BUILD_VERSION""*/);
		ImGui::Text("Created by Dallas Hardwicke");
		ImGui::Spacing();
		// TODO: Get special message for IJG copyright
		ImGui::Text(
			"Special thanks to:\n\t- Irrlicht\n\t- NVIDIA\n\t- Anax\n\t- AngelScript\n\t- DearImGui\n\t- Boost\n\t- Cereal\n\t- AssImp\n\t- IrrKlang\n\t- tinyXML\n\t- jpeglib\n\t- IJG\n\t- zlib\n\t- libPng");
		ImGui::Spacing();
		ImGui::Text(
			"ALL CODE CONTAINED HEREIN (THIS PROGRAM AND SUBSEQUENT LIBRARIES)\nIS PROPERTY OF ITS RIGHTFUL OWNER AND USED WITH PERMISSION WHERE NECCESARY");
		ImGui::Spacing();
	}
	ImGui::End();
}

void EditorInterface::loadEntityList()
{
	m_entityList.clear();

	for (auto tex : m_imguiEntityIconList)
	{
		RenderManager::Get()->imgui()->deleteTexture(tex);
	}
	m_imguiEntityIconList.clear();

	recursive_directory_iterator it("content/entity/"), end;

	std::vector<std::string> files;
	for (auto& entry : make_iterator_range(it, end))
	{
		if (is_regular(entry))
		{
			files.emplace_back(std::string(entry.path().native().begin(), entry.path().native().end()));
		}
	}

	m_imageEntityIconList.push_back(RenderManager::Get()->driver()->getTexture("content/texture/sprite/3x3.png"));
	m_imguiEntityIconList.push_back(RenderManager::Get()->imgui()->createTexture(m_imageEntityIconList.back()));

	m_imageEntityIconList.clear();

	for (const auto& file : files)
	{
		auto fname = file.substr(file.find_first_of('\\') + 1, std::string::npos);
		auto fname_noext = fname.substr(0, fname.size() - 4);

		m_entityList.emplace_back(fname_noext);
	}
}

void EditorInterface::loadPrefabList()
{
	m_prefabList.clear();

	recursive_directory_iterator it("content/prefab/"), end;

	std::vector<std::string> files;
	for (auto& entry : make_iterator_range(it, end))
	{
		if (is_regular(entry))
		{
			files.emplace_back(std::string(entry.path().native().begin(), entry.path().native().end()));
		}
	}

	for (const auto& file : files)
	{
		auto fname = file.substr(file.find_first_of('\\') + 1, std::string::npos);
		auto fname_noext = fname.substr(0, fname.size() - 4);

		m_prefabList.emplace_back(fname_noext);
	}
}

void EditorInterface::loadTextureList()
{
	m_textureList.clear();

	for (auto tex : m_imguiTextureList)
	{
		RenderManager::Get()->imgui()->deleteTexture(tex);
	}
	m_imguiTextureList.clear();

	recursive_directory_iterator it("content/texture/"), end;

	std::vector<std::string> files;
	for (auto& entry : make_iterator_range(it, end))
	{
		if (is_regular(entry))
		{
			files.emplace_back(std::string(entry.path().native().begin(), entry.path().native().end()));
		}
	}

	for (const auto& file : files)
	{
		auto fname = file.substr(file.find_first_of('\\') + 1, std::string::npos);
		auto fname_noext = fname.substr(0, fname.size() - 4);

		m_imageTextureFileList.push_back(
			RenderManager::Get()->driver()->getTexture(std::string("content/texture/" + fname).c_str()));
		m_imguiTextureList.push_back(RenderManager::Get()->imgui()->createTexture(m_imageTextureFileList.back()));

		m_textureList.emplace_back(fname_noext);
	}

	// DEBUG: Textures must NOT be dropped or else the won't exist in GPU memory for imgui
	//        IRR will not reload them from disk, it keeps them in memory when this function loops   
	/*for (auto img : m_imageTextureFileList) {
	    img->drop();
	}*/
	m_imageTextureFileList.clear();
}

void EditorInterface::loadMeshList()
{
	m_meshList.clear();

	recursive_directory_iterator it("content/mesh/"), end;

	std::vector<std::string> files;
	for (auto& entry : make_iterator_range(it, end))
	{
		if (is_regular(entry))
		{
			files.emplace_back(std::string(entry.path().native().begin(), entry.path().native().end()));
		}
	}

	for (const auto& file : files)
	{
		auto fname = file.substr(file.find_first_of('\\') + 1, std::string::npos);
		auto fname_noext = fname.substr(0, fname.size() - 4);

		if (fname.substr(fname.size() - 3) == "b3d")
		{
			m_meshList.emplace_back(fname_noext);
		}
	}
}

void EditorInterface::draw_window_spawn_entity()
{
	if (!m_windowData.draw_window_spawn_entity)
	{
		m_hasLoadedEntityList = false;
		return;
	}

	if (!m_hasLoadedEntityList)
	{
		loadEntityList();
		m_hasLoadedEntityList = true;
	}

	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(250, 500));
	if (ImGui::Begin("Entity Spawn Menu", &m_windowData.draw_window_spawn_entity, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::vector<std::string> folder;

		bool cont = false;
		for (const auto& path : m_entityList)
		{
			for (const auto& subpath : folder)
			{
				if (path.substr(0, path.find_first_of('\\')) == subpath)
				{
					cont = true;

					break;
				}

				cont = false;
			}

			if (!cont)
			{
				folder.emplace_back(path.substr(0, path.find_first_of('\\')));
			}
		}

		for (const auto& subpath : folder)
		{
			if (ImGui::CollapsingHeader(subpath.c_str()))
			{
				for (const auto& path : m_entityList)
				{
					if (path.substr(0, path.find_first_of('\\')) == subpath)
					{
						if (ImGui::Button(path.substr(path.find_last_of('\\') + 1).c_str()))
						{
							WorldManager::Get()->spawnEntity(_asset_ent(path));
							g_sceneInteractor.selectNewSpawnedEntityNextFrame();
						}
					}
				}
			}
		}
	}
	ImGui::End();
}

void EditorInterface::draw_window_spawn_prefab()
{
	//if (!m_windowData.draw_window_spawn_prefab) {
	//    m_hasLoadedPrefabList = false;
	//    return;
	//}

	//if (!m_hasLoadedPrefabList) {
	//    loadPrefabList();
	//    m_hasLoadedPrefabList = true;
	//}

	//ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(250, 700));
	//if (ImGui::Begin("Prefab Spawn Menu", &m_windowData.draw_window_spawn_prefab, ImGuiWindowFlags_AlwaysAutoResize)) {
	//    for (const auto& ename : m_prefabList) {
	//        if (ImGui::Button(ename.c_str())) {
	//            /*entityid spawn = */WorldManager::Get()->spawnPrefab(_asset_pre(ename));
	//            //g_sceneInteractor.setSelectedEntity(spawn);
	//            //g_currentEntity = spawn;
	//        }
	//    }
	//    /* if (ImGui::Button(noext.c_str())) {
	//         g_sceneInteractor.setSelectedEntity(WorldManager::Get()->spawnEntity(file.c_str()));
	//     }*/
	//}
	//ImGui::End();
}

void EditorInterface::draw_window_spawn_mesh()
{
	/*if (!m_windowData.draw_window_spawn_mesh) {
	    m_hasLoadedMeshList = false;

	    return;
	}

	if (!m_hasLoadedMeshList) {
	    loadMeshList();
	    m_hasLoadedMeshList = true;
	}

	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(250, 700));
	if (ImGui::Begin("Mesh Spawn Menu", &m_windowData.draw_window_spawn_mesh, ImGuiWindowFlags_AlwaysAutoResize)) {
	    for (const auto& ename : m_meshList) {
	        if (ImGui::Button(ename.c_str())) {
	            WorldManager::Get()->spawnStaticMesh(_asset_b3d(ename));
	        }
	    }
	}
	ImGui::End();*/
}

void EditorInterface::draw_window_texture_browser()
{
	if (!m_windowData.draw_window_texture_browser)
	{
		return;
	}

	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(344, 693));
	if (ImGui::Begin("Texture Browser"))
	{
		auto count = 0U;

		g_currentSelectedTexture = "null";

		for (const auto& str : m_textureList)
		{
			if (ImGui::ImageButton(m_imguiTextureList.at(count), ImVec2(64, 64)))
			{
				g_currentSelectedTexture = str.c_str();
				m_windowData.draw_window_texture_browser = false;
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::TextUnformatted(str.c_str());
				ImGui::EndTooltip();
			}

			if (count % 4 || !count)
			{
				ImGui::SameLine();
			}

			count++;
		}
	}
	ImGui::End();
}

void EditorInterface::show_window_texture_browser()
{
	if (!m_hasLoadedTextureList)
	{
		loadTextureList();
		m_hasLoadedTextureList = true;
	}

	m_windowData.draw_window_texture_browser = true;
}

void EditorInterface::draw_window_scene_stats()
{
	if (!m_windowData.draw_window_scene_stats) { return; }

	if (ImGui::Begin("Scene Stats", &m_windowData.draw_window_scene_stats, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(
			"FPS: %i\n"
			"Tri: %i\n\n"
			"Entities: %i",
			RenderManager::Get()->driver()->getFPS(),
			RenderManager::Get()->driver()->getPrimitiveCountDrawn(),
			WorldManager::Get()->world()->getEntityCount());
	}
	ImGui::End();
}

void EditorInterface::draw_window_console()
{
	if (!m_windowData.draw_window_console) { return; }

	//ImGui::End();
}

void EditorInterface::draw_window_log()
{
	if (!m_windowData.draw_window_log) { return; }

	if (ImGui::Begin("Log", &m_windowData.draw_window_log))
	{
		// Janky, very much so

		//ImGui::Text(g_LogOuputStream.str().c_str());
	}
	ImGui::End();
}

void EditorInterface::draw_window_editor_settings()
{
	static bool open = false;

	static EditorConfiguration config = g_sceneInteractor.getConfiguration();

	if (!m_windowData.draw_window_editor_settings) { return; }

	if (ImGui::Begin("Editor Settings", &m_windowData.draw_window_editor_settings, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (!open) {
			config = g_sceneInteractor.getConfiguration();
			open = true;
		}

		ImGui::Checkbox("Draw Light Range Sphere", &config.drawPointLightBounds);
		ImGui::Checkbox("Snap to Grid", &config.useSnap);
		ImGui::InputFloat("Snap Value", &config.snapX, 0.05, 0.25, 3, 0);
		config.snapZ = config.snapY = config.snapX;

		if (ImGui::Button("Save"))
		{
			g_sceneInteractor.saveConfiguration(config);
			m_windowData.draw_window_editor_settings = false;
			open = false;
		}
		if (ImGui::Button("Cancel"))
		{
			m_windowData.draw_window_editor_settings = false;
			open = false;
		}
	}
	ImGui::End();
}

void EditorInterface::draw_window_add_component()
{
	if (!m_windowData.draw_window_add_component) { return; }

	if (ImGui::Begin("Add Component", &m_windowData.draw_window_add_component, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static int current_selected_component = 0;
		const char* component_list =
			"Auto Kill\0"
			"Billboard Sprite\0"
			"Camera\0"
			"Character Controller\0"
			"Damage Receiver\0"
			"Data\0"
			"Debug Mesh\0"
			"Debug Sprite\0"
			"Descriptor\0"
			"Interaction\0"
			"Item\0"
			"Light\0"
			"Logic\0"
			"Marker\0"
			"Mesh\0"
			"NPC\0"
			"Physics\0"
			"Prefab\0"
			"Render\0"
			"Script\0"
			"Sound\0"
			"Sound Listener\0"
			"Transform\0"
			"Trigger Zone\0\0"; // 24

		ImGui::Combo("Component", &current_selected_component, component_list, 24);
		ImGui::SameLine();
		//ImGui::Spacing();

		/*if (draw_component_properties(static_cast<ENTITY_COMPONENT>(current_selected_component),
		                              WorldManager::Get()->managerSystem()->getEntityByID(g_currentEntity)))
		{
			ImGui::Spacing();

			ImGui::Text("Entity already contains selected component");

			ImGui::Spacing();
		}
		else*/
		{
			//ImGui::Spacing();

			if (ImGui::Button("Add"))
			{
				auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(g_currentEntity);

				if (entity.isValid())
				{
					switch (static_cast<ENTITY_COMPONENT>(current_selected_component))
					{
					case ENTITY_COMPONENT::BILLBOARDSPRITE:
						if (entity.hasComponent<BillboardSpriteComponent>())
						{
							break;
						}
						entity.addComponent<BillboardSpriteComponent>();
						break;
					case ENTITY_COMPONENT::CAMERA:
						if (entity.hasComponent<CameraComponent>())
						{
							break;
						}
						entity.addComponent<CameraComponent>();
						break;
					case ENTITY_COMPONENT::CHARACTERCONTROLLER:
						if (entity.hasComponent<CCTComponent>())
						{
							break;
						}
						entity.addComponent<CCTComponent>();
						break;
					case ENTITY_COMPONENT::DEBUGMESH:
						if (entity.hasComponent<DebugMeshComponent>())
						{
							break;
						}
						entity.addComponent<DebugMeshComponent>();
						break;
					case ENTITY_COMPONENT::DEBUGSPRITE:
						if (entity.hasComponent<DebugSpriteComponent>())
						{
							break;
						}
						entity.addComponent<DebugSpriteComponent>();
						break;
					case ENTITY_COMPONENT::DESCRIPTOR:
						if (entity.hasComponent<DescriptorComponent>())
						{
							break;
						}
						entity.addComponent<DescriptorComponent>();
						break;
					case ENTITY_COMPONENT::LIGHT:
						if (entity.hasComponent<LightComponent>())
						{
							break;
						}
						entity.addComponent<LightComponent>();
						break;
					case ENTITY_COMPONENT::MESH:
						if (entity.hasComponent<MeshComponent>())
						{
							break;
						}
						entity.addComponent<MeshComponent>();
						break;
					case ENTITY_COMPONENT::PHYSICS:
						if (entity.hasComponent<PhysicsComponent>())
						{
							break;
						}
						entity.addComponent<PhysicsComponent>();
						break;
					case ENTITY_COMPONENT::PREFAB:
						if (entity.hasComponent<PrefabComponent>())
						{
							break;
						}
						entity.addComponent<PrefabComponent>();
						break;
					case ENTITY_COMPONENT::RENDER:
						if (entity.hasComponent<RenderComponent>())
						{
							break;
						}
						entity.addComponent<RenderComponent>();
						break;
					case ENTITY_COMPONENT::SCRIPT:
						if (entity.hasComponent<ScriptComponent>())
						{
							break;
						}
						entity.addComponent<ScriptComponent>();
						break;
					case ENTITY_COMPONENT::SOUND:
						if (entity.hasComponent<SoundComponent>())
						{
							break;
						}
						entity.addComponent<SoundComponent>();
						break;
					case ENTITY_COMPONENT::SOUNDLISTENER:
						if (entity.hasComponent<SoundListenerComponent>())
						{
							break;
						}
						entity.addComponent<SoundListenerComponent>();
						break;
					case ENTITY_COMPONENT::TRANSFORM:
						if (entity.hasComponent<TransformComponent>())
						{
							break;
						}
						entity.addComponent<TransformComponent>();
						break;
					case ENTITY_COMPONENT::AUTOKILL:
						if (entity.hasComponent<AutoKillComponent>())
						{
							break;
						}
						entity.addComponent<AutoKillComponent>();
						break;
					case ENTITY_COMPONENT::DAMAGERECEIVER:
						if (entity.hasComponent<DamageReceiverComponent>())
						{
							break;
						}
						entity.addComponent<DamageReceiverComponent>();
						break;
					case ENTITY_COMPONENT::DATA:
						if (entity.hasComponent<DataComponent>())
						{
							break;
						}
						entity.addComponent<DataComponent>();
						break;
					case ENTITY_COMPONENT::INTERACTION:
						if (entity.hasComponent<InteractionComponent>())
						{
							break;
						}
						entity.addComponent<InteractionComponent>();
						break;
					case ENTITY_COMPONENT::ITEM:
						if (entity.hasComponent<ItemComponent>())
						{
							break;
						}
						entity.addComponent<ItemComponent>();
						break;
					case ENTITY_COMPONENT::LOGIC:
						if (entity.hasComponent<LogicComponent>())
						{
							break;
						}
						entity.addComponent<LogicComponent>();
						break;
					case ENTITY_COMPONENT::MARKER:
						if (entity.hasComponent<MarkerComponent>())
						{
							break;
						}
						entity.addComponent<MarkerComponent>();
						break;
					case ENTITY_COMPONENT::NPC:
						if (entity.hasComponent<NPCComponent>())
						{
							break;
						}
						entity.addComponent<NPCComponent>();
						break;
					case ENTITY_COMPONENT::TRIGGERZONE:
						if (entity.hasComponent<TriggerZoneComponent>())
						{
							break;
						}
						entity.addComponent<TriggerZoneComponent>();
						break;
					}

					WorldManager::Get()->world()->refresh();
				}
				else
				{
					// Error...
				}

				m_windowData.draw_window_add_component = false;
			}

			ImGui::SameLine();
		}

		if (ImGui::Button("Cancel"))
		{
			m_windowData.draw_window_add_component = false;
		}
	}
	ImGui::End();
}

bool EditorInterface::draw_component_properties(ENTITY_COMPONENT component, anax::Entity& entity)
{
	switch (component)
	{
	case ENTITY_COMPONENT::AUTOKILL:
		{
			if (!entity.hasComponent<AutoKillComponent>())
			{
				return false;
			}

			ImGui::Text("Component contains no adjustable properties");

			break;
		}
	case ENTITY_COMPONENT::BILLBOARDSPRITE:
		{
			if (!entity.hasComponent<BillboardSpriteComponent>())
			{
				return false;
			}

			auto& billboardsprite = entity.getComponent<BillboardSpriteComponent>();

			ImGui::Checkbox("Visible", &billboardsprite.isVisible);
			ImGui::Checkbox("Animated", &billboardsprite.animated);

			if (billboardsprite.animated)
			{
				ImGui::Checkbox("Loop", &billboardsprite.loop);

				ImGui::Spacing();

				ImGui::InputInt("FPS", &billboardsprite.fps);
				ImGui::InputInt("Frame Width", &billboardsprite.split_x);
				ImGui::InputInt("Frame Height", &billboardsprite.split_y);
			}
			else
			{
				ImGui::Spacing();
			}
			ImGui::InputFloat("Scale X", &billboardsprite.scale_x);
			ImGui::InputFloat("Scale Y", &billboardsprite.scale_y);

			ImGui::Spacing();

			auto tstr = billboardsprite.sprite;
			char buf[256];
			static bool is_searching = false;

			memset(buf, '\0', 256);

			for (auto c = 0U; c < tstr.size(); c++)
			{
				buf[c] = tstr[c];
			}
			ImGui::PushID("textureInputManager::Get()_");
			if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				tstr = buf;
				billboardsprite.node->setMaterialTexture(0, RenderManager::Get()->driver()->getTexture(tstr.c_str()));
			}
			ImGui::PopID();

			ImGui::SameLine();

			ImGui::PushID("texture_browse_");
			if (ImGui::Button("..."))
			{
				show_window_texture_browser();
				is_searching = true;
			}
			ImGui::PopID();

			if (is_searching && g_currentSelectedTexture != "null")
			{
				billboardsprite.sprite = _asset_tex(g_currentSelectedTexture);
				billboardsprite.node->setMaterialTexture(
					0, RenderManager::Get()->driver()->getTexture(billboardsprite.sprite.c_str()));

				is_searching = false;
				g_currentSelectedTexture = "null";
			}

			break;
		}
	case ENTITY_COMPONENT::CAMERA:
		{
			if (!entity.hasComponent<CameraComponent>())
			{
				return false;
			}

			auto& camera = entity.getComponent<CameraComponent>();

			float fl3_off[3];
			camera.offset.getAs3Values(fl3_off);
			if (ImGui::InputFloat3("Positon", fl3_off, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				camera.offset = irr::core::vector3df(fl3_off[0], fl3_off[1], fl3_off[2]);
			}
			float fl3_tar[3];
			camera.target.getAs3Values(fl3_tar);
			if (ImGui::InputFloat3("Rotation", fl3_tar, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				camera.target = irr::core::vector3df(fl3_tar[0], fl3_tar[1], fl3_tar[2]);
			}

			break;
		}
	case ENTITY_COMPONENT::CHARACTERCONTROLLER:
		{
			if (!entity.hasComponent<CCTComponent>())
			{
				return false;
			}

			auto& cct = entity.getComponent<CCTComponent>();

			if (ImGui::Checkbox("Active", &cct.active))
			{
				WorldManager::Get()->renderSystem()->forceTransformUpdate();
			}

			break;
		}
	case ENTITY_COMPONENT::DAMAGERECEIVER:
		{
			if (!entity.hasComponent<DamageReceiverComponent>())
			{
				return false;
			}

			ImGui::Text("Component contains no adjustable properties");

			break;
		}
	case ENTITY_COMPONENT::DATA:
		{
			if (!entity.hasComponent<DataComponent>())
			{
				return false;
			}

			auto& data = entity.getComponent<DataComponent>();

			char buffer[256];
			memset(buffer, 0, sizeof buffer);

			for (auto i = 0U; i < data.data.size(); i++)
			{
				for (auto n = 0U; n < data.data.at(i).length(); n++)
				{
					buffer[n] = data.data.at(i).at(n);
				}

				ImGui::PushID(std::to_string(i).c_str());
				if (ImGui::Button(" - "))
				{
					data.data.erase(data.data.begin() + i);
				}
				ImGui::PopID();

				ImGui::SameLine();

				if (ImGui::InputText(std::string("Slot " + std::to_string(i)).c_str(), buffer, sizeof buffer,
				                     ImGuiInputTextFlags_EnterReturnsTrue))
				{
					data.data.at(i) = buffer;
				}

				memset(buffer, 0, sizeof buffer);
			}

			if (ImGui::Button("Add Slot"))
			{
				data.data.emplace_back(std::string());
			}

			break;
		}
	case ENTITY_COMPONENT::DEBUGMESH:
		{
			if (!entity.hasComponent<DebugMeshComponent>())
			{
				return false;
			}

			auto& debugmesh = entity.getComponent<DebugMeshComponent>();

			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < debugmesh.mesh.size() && i < 256; i++)
			{
				buf[i] = debugmesh.mesh[i];
			}
			if (ImGui::InputText("Mesh", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				debugmesh.mesh = buf;
				WorldManager::Get()->renderSystem()->setDebugMeshComponentData(entity);
			}

			auto tstr = debugmesh.texture;
			static bool is_searching = false;

			memset(buf, 0, 256);

			for (auto c = 0U; c < tstr.size(); c++)
			{
				buf[c] = tstr[c];
			}
			ImGui::PushID("textureInputManager::Get()_");
			if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				tstr = buf;
				debugmesh.node->setMaterialTexture(0, RenderManager::Get()->driver()->getTexture(tstr.c_str()));
			}
			ImGui::PopID();

			ImGui::SameLine();

			ImGui::PushID("texture_browse_");
			if (ImGui::Button("..."))
			{
				show_window_texture_browser();
				is_searching = true;
			}
			ImGui::PopID();

			if (is_searching && g_currentSelectedTexture != "null")
			{
				debugmesh.texture = _asset_tex(g_currentSelectedTexture);
				debugmesh.node->setMaterialTexture(
					0, RenderManager::Get()->driver()->getTexture(debugmesh.texture.c_str()));

				is_searching = false;
				g_currentSelectedTexture = "null";
			}

			break;
		}
	case ENTITY_COMPONENT::DEBUGSPRITE:
		{
			if (!entity.hasComponent<DebugSpriteComponent>())
			{
				return false;
			}

			auto& debugsprite = entity.getComponent<DebugSpriteComponent>();

			auto tstr = debugsprite.sprite;
			char buf[256];
			static bool is_searching = false;

			memset(buf, '\0', 256);

			for (auto c = 0U; c < tstr.size(); c++)
			{
				buf[c] = tstr[c];
			}
			ImGui::PushID("textureInputManager::Get()_");
			if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				tstr = buf;
				debugsprite.node->setMaterialTexture(0, RenderManager::Get()->driver()->getTexture(tstr.c_str()));
			}
			ImGui::PopID();

			ImGui::SameLine();

			ImGui::PushID("texture_browse_");
			if (ImGui::Button("..."))
			{
				show_window_texture_browser();
				is_searching = true;
			}
			ImGui::PopID();

			if (is_searching && g_currentSelectedTexture != "null")
			{
				debugsprite.sprite = _asset_tex(g_currentSelectedTexture);
				debugsprite.node->setMaterialTexture(
					0, RenderManager::Get()->driver()->getTexture(debugsprite.sprite.c_str()));

				is_searching = false;
				g_currentSelectedTexture = "null";
			}

			break;
		}
	case ENTITY_COMPONENT::DESCRIPTOR:
		{
			if (!entity.hasComponent<DescriptorComponent>())
			{
				return false;
			}

			auto& descriptor = entity.getComponent<DescriptorComponent>();

			ImGui::Text("ID: %d", descriptor.id);

			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < descriptor.name.size() && i < 256; i++)
			{
				buf[i] = descriptor.name[i];
			}
			ImGui::PushID("DescriptorNameInput");
			if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				descriptor.name = buf;
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text("Name");

			std::string str_type;

			switch (descriptor.type)
			{
			case ET_NULL:
				str_type = "NULL";
				break;
			case ET_STATIC:
				str_type = "STATIC";
				break;
			case ET_DYNAMIC:
				str_type = "DYNAMIC";
				break;
			case ET_PLAYER:
				str_type = "PLAYER";
				break;
			default:
				str_type = "NULL";
				break;
			}

			int ect = static_cast<unsigned int>(descriptor.type);
			ImGui::PushID("DescriptorType");
			if (ImGui::InputInt("", &ect, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (ect > 2)
				{
					ect = 2;
				}
				if (ect < 0)
				{
					ect = 0;
				}
				descriptor.type = static_cast<ENTITY_TYPE>(ect);
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text(std::string("Type: " + str_type).c_str());

			break;
		}
	case ENTITY_COMPONENT::INTERACTION:
		{
			if (!entity.hasComponent<InteractionComponent>())
			{
				return false;
			}

			ImGui::Text("Component contains no adjustable properties");

			break;
		}
	case ENTITY_COMPONENT::ITEM:
		{
			if (!entity.hasComponent<ItemComponent>())
			{
				return false;
			}

			ImGui::Text("Component contains no adjustable properties");

			break;
		}
	case ENTITY_COMPONENT::LIGHT:
		{
			if (!entity.hasComponent<LightComponent>())
			{
				return false;
			}

			auto& light = entity.getComponent<LightComponent>();

			ImGui::InputFloat("Radius", &light.radius, 0.1f, 0.0f, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			//ImGui::InputFloat("Inner Cone", &light.innerCone, 0.1f, 0.0f, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			if (light.type == LT_SPOT)
			{
				ImGui::InputFloat("Outer Cone", &light.outerCone, 0.1f, 0.0f, 2, ImGuiInputTextFlags_EnterReturnsTrue);
				ImGui::InputFloat("Attenuation", &light.falloff, 0.1f, 0.0f, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			}

			ImColor color(light.color_diffuse.r, light.color_diffuse.g, light.color_diffuse.b, light.color_diffuse.a);
			ImColorPicker("", &color);
			light.color_diffuse.r = color.Value.x;
			light.color_diffuse.g = color.Value.y;
			light.color_diffuse.b = color.Value.z;
			light.color_diffuse.a = color.Value.w;

			light.update_component_data = true;
			WorldManager::Get()->renderSystem()->forceTransformUpdate();

			break;
		}
	case ENTITY_COMPONENT::LOGIC:
		{
			if (!entity.hasComponent<LogicComponent>())
			{
				return false;
			}

			auto& logic = entity.getComponent<LogicComponent>();

			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < logic.receiver.size() && i < 256; i++)
			{
				buf[i] = logic.receiver[i];
			}
			if (ImGui::InputText("Event Receiver", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				logic.receiver = buf;
			}

			break;
		}
	case ENTITY_COMPONENT::MARKER:
		{
			if (!entity.hasComponent<MarkerComponent>())
			{
				return false;
			}

			auto& marker = entity.getComponent<MarkerComponent>();

			std::string str_type;

			switch (marker.type)
			{
			case MARKER_TYPE::MT_NULL:
				str_type = "MT_NULL";
				break;
			case MARKER_TYPE::MT_PLAYER_START:
				str_type = "MT_PLAYER_START";
				break;
			case MARKER_TYPE::MT_FREECAMERA:
				str_type = "MT_FREECAMERA_START";
				break;
			}

			ImGui::Text(std::string("Type: " + str_type).c_str());

			break;
		}
	case ENTITY_COMPONENT::MESH:
		{
			if (!entity.hasComponent<MeshComponent>())
			{
				return false;
			}

			auto& mesh = entity.getComponent<MeshComponent>();
			{
				char buf[256];
				memset(buf, 0, 256);
				for (auto i = 0U; i < mesh.mesh.size() && i < 256; i++)
				{
					buf[i] = mesh.mesh[i];
				}
				ImGui::PushID("mesh_file");
				if (ImGui::InputText("Mesh", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					mesh.mesh = buf;
					WorldManager::Get()->renderSystem()->setMeshComponentData(entity);
				}
				ImGui::PopID();
			}

			ImGui::Checkbox("Cast Shadows   ", &mesh.castShadows);
			ImGui::Checkbox("Receive Shadows", &mesh.receiveShadows);
			/*if (mesh.textures.size())
			{*/
				ImGui::Text("Texture(s):");

				char buf[256];

				static int index = 0;
				static bool is_searching = false;

				auto iter = 0U;
				for (auto& tstr : mesh.textures)
				{
					memset(buf, '\0', 256);

					ImGui::PushID(("texture_remove" + std::to_string(iter)).c_str());
					if (ImGui::Button(" - "))
					{
						if (iter > 0)
						{
							mesh.textures.erase(mesh.textures.begin() + iter);
						}
					}
					ImGui::PopID();

					ImGui::SameLine();

					for (auto c = 0U; c < tstr.size(); c++)
					{
						buf[c] = tstr[c];
					}
					ImGui::PushID(("textureInputManager::Get()_" + std::to_string(iter)).c_str());
					if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						tstr = buf;
						mesh.node->setMaterialTexture(iter, RenderManager::Get()->driver()->getTexture(tstr.c_str()));
					}
					ImGui::PopID();

					ImGui::SameLine();

					ImGui::PushID(("texture_browse_" + std::to_string(iter)).c_str());
					if (ImGui::Button("..."))
					{
						show_window_texture_browser();
						is_searching = true;
						index = iter;
					}
					ImGui::PopID();

					iter++;
				}

				if (is_searching && g_currentSelectedTexture != "null")
				{
					mesh.textures.at(index) = _asset_tex(g_currentSelectedTexture);
					mesh.node->setMaterialTexture(
						index, RenderManager::Get()->driver()->getTexture(mesh.textures.at(index).c_str()));

					index = 0;
					is_searching = false;
					g_currentSelectedTexture = "null";
				}

				if (ImGui::Button("Add"))
				{
					if (mesh.textures.size() < _IRR_MATERIAL_MAX_TEXTURES_)
					{
						mesh.textures.emplace_back(std::string());
					}
				}
			/*}
			else
			{
				ImGui::Text(" - Texture(s) are assigned by the mesh - ");
			}*/

			ImGui::Spacing();

			int mtype = static_cast<unsigned int>(mesh.renderMaterial);
			ImGui::PushID("render_material_type");
			if (ImGui::InputInt("", &mtype, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (mtype > 24)
				{
					mtype = 0;
				}
				if (mtype < 0)
				{
					mtype = 0;
				}

				mesh.renderMaterial = static_cast<irr::video::E_MATERIAL_TYPE>(mtype);

				for (irr::u32 i = 0; i < mesh.node->getMaterialCount(); i++)
				{
					mesh.node->getMaterial(i).MaterialType = mesh.renderMaterial;
				}
				RenderManager::Get()->renderer()->getMaterialSwapper()->swapMaterials(mesh.node);
			}
			ImGui::PopID();

			std::string mask_type;

			switch (mesh.renderMaterial)
			{
			case irr::video::E_MATERIAL_TYPE::EMT_SOLID:
				mask_type = "EMT_SOLID";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_SOLID_2_LAYER:
				mask_type = "EMT_SOLID_2_LAYER";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP:
				mask_type = "EMT_LIGHTMAP";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_ADD:
				mask_type = "EMT_LIGHTMAP_ADD";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_M2:
				mask_type = "EMT_LIGHTMAP_M2";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_M4:
				mask_type = "EMT_LIGHTMAP_M4";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING:
				mask_type = "EMT_LIGHTMAP_LIGHTING";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING_M2:
				mask_type = "EMT_LIGHTMAP_LIGHTING_M2";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING_M4:
				mask_type = "EMT_LIGHTMAP_LIGHTING_M4";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_DETAIL_MAP:
				mask_type = "EMT_DETAIL_MAP";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_SPHERE_MAP:
				mask_type = "EMT_SPHERE_MAP";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_REFLECTION_2_LAYER:
				mask_type = "EMT_REFLECTION_2_LAYER";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ADD_COLOR:
				mask_type = "EMT_TRANSPARENT_ADD_COLOR";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL:
				mask_type = "EMT_TRANSPARENT_ALPHA_CHANNEL";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
				mask_type = "EMT_TRANSPARENT_ALPHA_CHANNEL_REF";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_VERTEX_ALPHA:
				mask_type = "EMT_TRANSPARENT_VERTEX_ALPHA";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_REFLECTION_2_LAYER:
				mask_type = "EMT_TRANSPARENT_REFLECTION_2_LAYER";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_SOLID:
				mask_type = "EMT_NORMAL_MAP_SOLID";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
				mask_type = "EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
				mask_type = "EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_SOLID:
				mask_type = "EMT_PARALLAX_MAP_SOLID";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
				mask_type = "EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
				mask_type = "EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA";
				break;
			case irr::video::E_MATERIAL_TYPE::EMT_ONETEXTURE_BLEND:
				mask_type = "EMT_ONETEXTURE_BLEND";
				break;
			default:
				mask_type = "NULL";
				break;
			}
			ImGui::Text("Material Type: %s", mask_type.c_str());

			ImGui::Spacing();

			if (mesh.isAnimated)
			{
				ImGui::InputInt("FPS", &mesh.fps, 1.f, 1.f, 1);
			}

			if (mesh.animationList.size() > 0)
			{
				if (ImGui::TreeNode("Animations"))
				{
					for (auto an : mesh.animationList)
					{
						ImGui::Text("%s: %i, %i", an.name.c_str(), static_cast<unsigned int>(an.frames.X),
						            static_cast<unsigned int>(an.frames.Y));
					}

					ImGui::TreePop();
				}
			}

			break;
		}
	case ENTITY_COMPONENT::NPC:
		{
			if (!entity.hasComponent<NPCComponent>())
			{
				return false;
			}

			auto& npc = entity.getComponent<NPCComponent>();

			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < npc.name.length() && i < 256; i++)
			{
				buf[i] = npc.name[i];
			}
			if (ImGui::InputText("Display Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				npc.name = buf;
			}

			ImGui::Spacing();

			ImGui::InputFloat("Vision Range", &npc.visionRange, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::InputFloat("Chase Range", &npc.chaseRange, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::InputFloat("Attack Range", &npc.attackRange, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::InputFloat("Attack Delay", &npc.attackDelay, 1, 10, 2, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::InputFloat("Move Speed", &npc.moveSpeed, 1, 10, 3, ImGuiInputTextFlags_EnterReturnsTrue);

			ImGui::Spacing();

			int mtype = static_cast<unsigned int>(npc.state);
			ImGui::PushID(1);
			if (ImGui::InputInt("", &mtype, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (mtype > 7)
				{
					mtype = 0;
				}
				if (mtype < 0)
				{
					mtype = 0;
				}

				npc.state = static_cast<NPC_AI_STATE>(mtype);
			}
			ImGui::PopID();

			std::string mask_type;

			switch (npc.state)
			{
			case NPC_AI_STATE::INACTIVE:
				mask_type = "INACTIVE";
				break;
			case NPC_AI_STATE::IDLE:
				mask_type = "IDLE";
				break;
			case NPC_AI_STATE::PATROL:
				mask_type = "PATROL";
				break;
			case NPC_AI_STATE::ALERT:
				mask_type = "ALERT";
				break;
			case NPC_AI_STATE::ATTACK:
				mask_type = "ATTACK";
				break;
			case NPC_AI_STATE::CHASE:
				mask_type = "CHASE";
				break;
			case NPC_AI_STATE::FLEE:
				mask_type = "FLEE";
				break;
			case NPC_AI_STATE::DEAD:
				mask_type = "DEAD";
				break;
			default:
				mask_type = "NULL";
				break;
			}
			ImGui::SameLine();
			ImGui::Text("AI State: %s", mask_type.c_str());

			ImGui::Spacing();

			char buf2[256];
			memset(buf2, 0, 256);
			for (auto i = 0U; i < npc.start_waypoint.length() && i < 256; i++)
			{
				buf2[i] = npc.start_waypoint[i];
			}
			if (ImGui::InputText("Initial Waypoint", buf2, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				npc.start_waypoint = buf2;
			}

			char buf3[256];
			memset(buf3, 0, 256);
			for (auto i = 0U; i < npc.current_waypoint.length() && i < 256; i++)
			{
				buf3[i] = npc.current_waypoint[i];
			}
			if (ImGui::InputText("Current Waypoint", buf3, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				npc.current_waypoint = buf3;
			}

			break;
		}
	case ENTITY_COMPONENT::PHYSICS:
		{
			if (!entity.hasComponent<PhysicsComponent>())
			{
				return false;
			}

			auto& physics = entity.getComponent<PhysicsComponent>();

			std::string str_type;

			switch (physics.type)
			{
			case PHYSICS_COLLIDER_TYPE::PCT_BOX:
				str_type = "BOX";
				break;
			case PHYSICS_COLLIDER_TYPE::PCT_CAPSULE:
				str_type = "CAPSULE";
				break;
			case PHYSICS_COLLIDER_TYPE::PCT_CONVEX:
				str_type = "CONVEX";
				break;
			case PHYSICS_COLLIDER_TYPE::PCT_PLANE:
				str_type = "PLANE";
				break;
			case PHYSICS_COLLIDER_TYPE::PCT_SPHERE:
				str_type = "SPHERE";
				break;
			case PHYSICS_COLLIDER_TYPE::PCT_TRIANGLE:
				str_type = "TRIANGLE";
				break;
			default:
				str_type = "NULL";
				break;
			}

			int pct = static_cast<unsigned int>(physics.type);
			ImGui::PushID("PhysicsType");
			if (ImGui::InputInt("", &pct, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (pct > 5)
				{
					pct = 5;
				}
				if (pct < 0)
				{
					pct = 0;
				}
				physics.type = static_cast<PHYSICS_COLLIDER_TYPE>(pct);
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Text(std::string("Collider: " + str_type).c_str());

			if (physics.type == PHYSICS_COLLIDER_TYPE::PCT_TRIANGLE)
			{
				ImGui::PushID("CollisionMeshPathText");
				if (physics.collisionMesh.empty())
				{
					ImGui::Text("No Mesh Specified!");
				}
				else
				{
					ImGui::Text(physics.collisionMesh.c_str());
				}
				ImGui::PopID();

				ImGui::SameLine();

				ImGui::PushID("LoadCollsionMeshButton");
				if (ImGui::Button("Load..."))
				{
					auto mesh = Utility::OpenFileDialog(dialog_filter_mesh);

					std::size_t found = mesh.find("content\\");
					if (found != std::string::npos)
					{
						physics.collisionMesh = mesh.substr(found);
					}
					else
					{
						Utility::Warning("Assets must exist in local path!");
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Checkbox("Kinematic", &physics.kinematic);
			}

			if (physics.type != PHYSICS_COLLIDER_TYPE::PCT_TRIANGLE)
			{
				ImGui::DragFloat(std::string("Density").c_str(), &physics.density, 0.5f, 0.0f, 10.0, "%.0f");
			}

			break;
		}
	case ENTITY_COMPONENT::PREFAB:
		{
			if (!entity.hasComponent<PrefabComponent>())
			{
				return false;
			}

			auto& prefab = entity.getComponent<PrefabComponent>();

			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < prefab.parent.length() && i < 256; i++)
			{
				buf[i] = prefab.parent[i];
			}
			if (ImGui::InputText("Parent", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				prefab.parent = buf;

				if (prefab.parent.length() > 0)
				{
					prefab.isChild = true;
				}
				else
				{
					prefab.isChild = false;
				}
			}

			break;
		}
	case ENTITY_COMPONENT::RENDER:
		{
			if (!entity.hasComponent<RenderComponent>())
			{
				return false;
			}

			auto& render = entity.getComponent<RenderComponent>();

			if (ImGui::Checkbox("Visible", &render.isVisible))
			{
				WorldManager::Get()->renderSystem()->forceTransformUpdate();
			}

			break;
		}
	case ENTITY_COMPONENT::SCRIPT:
		{
			if (!entity.hasComponent<ScriptComponent>())
			{
				return false;
			}

			auto& script = entity.getComponent<ScriptComponent>();

			static bool invalid_script = false;
			char buf[256];
			memset(buf, 0, 256);
			for (auto i = 0U; i < script.script.size() && i < 256; i++)
			{
				buf[i] = script.script[i];
			}
			ImGui::Text("Script: ");
			ImGui::SameLine();
			ImGui::PushID("ScriptFile");
			if (ImGui::InputText("", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				script.script = buf;
				if (ScriptManager::Get()->compile(entity.getComponent<ScriptComponent>()) < 0)
				{
					invalid_script = true;
				}
				else
				{
					invalid_script = false;
				}
			}
			ImGui::PopID();

			if (invalid_script)
			{
				ImGui::SameLine();
				ImGui::Text(" -> Invalid script file!");
			}

			ImGui::Spacing();

			unsigned int i = 0;
			for (auto gbls : script.globals)
			{
				bool exposed = false;
				for (auto gl_idx : script.exposed_global_indexes)
				{
					if (gbls.first == gl_idx)
					{
						exposed = true;
					}
				}

				if (exposed)
				{
					std::string var_name = gbls.second.substr(gbls.second.find_first_of(' ') + 1);

					if (var_name.size() > 2)
					{
						if (var_name.at(0) == 'g' && var_name.at(1) == '_')
						{
							var_name = var_name.substr(2);
						}
					}

					switch (script.global_values.at(i).first)
					{
					case AS_DATA_TYPE::INT:
						{
							auto* value = reinterpret_cast<int*>(script.global_values.at(i).second);
							ImGui::InputInt(var_name.c_str(), value, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue);
							break;
						}
					case AS_DATA_TYPE::BOOL:
						{
							auto* value = reinterpret_cast<bool*>(script.global_values.at(i).second);
							ImGui::Checkbox(var_name.c_str(), value);
							break;
						}
					case AS_DATA_TYPE::FLOAT:
						{
							auto* value = reinterpret_cast<float*>(script.global_values.at(i).second);
							ImGui::InputFloat(var_name.c_str(), value, 0, 0, 2, ImGuiInputTextFlags_EnterReturnsTrue);
							break;
						}
					case AS_DATA_TYPE::STRING:
						{
							auto* value = reinterpret_cast<std::string*>(script.global_values.at(i).second);
							char strbuf[256];
							memset(strbuf, 0, 256);
							for (auto j = 0U; j < value->size() && j < 256; j++)
							{
								strbuf[j] = value->at(j);
							}
							if (ImGui::InputText(var_name.c_str(), strbuf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
							{
								value->assign(strbuf);
							}
							break;
						}
					case AS_DATA_TYPE::VECTOR2:
						{
							auto* value = reinterpret_cast<irr::core::vector2df*>(script.global_values.at(i).second);
							float f2v[2];
							f2v[0] = value->X;
							f2v[1] = value->Y;
							if (ImGui::InputFloat2(var_name.c_str(), f2v, 2, ImGuiInputTextFlags_EnterReturnsTrue))
							{
								value->X = f2v[0];
								value->Y = f2v[1];
							}
							break;
						}
					case AS_DATA_TYPE::VECTOR3:
						{
							auto* value = reinterpret_cast<irr::core::vector3df*>(script.global_values.at(i).second);
							float f3v[3];
							value->getAs3Values(f3v);
							if (ImGui::InputFloat3(var_name.c_str(), f3v, 2, ImGuiInputTextFlags_EnterReturnsTrue))
							{
								value->X = f3v[0];
								value->Y = f3v[1];
								value->Z = f3v[2];
							}
							break;
						}
					default:
						break;
					}
				}

				i++;
			}

			break;
		}
	case ENTITY_COMPONENT::SOUND:
		{
			if (!entity.hasComponent<SoundComponent>())
			{
				return false;
			}

			auto& sound = entity.getComponent<SoundComponent>();

			auto iter = 0U;
			for (auto& s : sound.sounds)
			{
				if (ImGui::TreeNode(s.name.c_str()))
				{
					// Check if name already exists
					char buf2[256];
					memset(buf2, 0, 256);
					for (auto i = 0U; i < s.name.size(); i++)
					{
						buf2[i] = s.name[i];
					}
					if (ImGui::InputText(std::string("Name    [" + std::to_string(iter) + "]").c_str(), buf2, 256,
					                     ImGuiInputTextFlags_EnterReturnsTrue))
					{
						std::string t = buf2;
						if (t.size() > 1)
						{
							s.name = buf2;
						}
					}


					char buf1[256];
					memset(buf1, 0, 256);
					for (auto i = 0U; i < s.file.size(); i++)
					{
						buf1[i] = s.file[i];
					}
					if (ImGui::InputText(std::string("File    [" + std::to_string(iter) + "]").c_str(), buf1, 256,
					                     ImGuiInputTextFlags_EnterReturnsTrue))
					{
						s.file = buf1;
					}

					ImGui::DragFloat(std::string("Volume  [" + std::to_string(iter) + "]").c_str(), &s.volume, 1.0f,
					                 0.0f, 100.0f, "%.0f");
					ImGui::DragFloat(std::string("MinDist [" + std::to_string(iter) + "]").c_str(), &s.minDist, 1.0f,
					                 0.1f, 100.0f, "%.0f");

					ImGui::Checkbox(std::string("3D      [" + std::to_string(iter) + "]").c_str(), &s.is3D);
					ImGui::Checkbox(std::string("Loop    [" + std::to_string(iter) + "]").c_str(), &s.loop);

					ImGui::TreePop();
				}

				iter++;
			}

			break;
		}
	case ENTITY_COMPONENT::SOUNDLISTENER:
		{
			if (!entity.hasComponent<SoundListenerComponent>())
			{
				return false;
			}

			ImGui::Text("Component contains no adjustable properties");

			break;
		}
	case ENTITY_COMPONENT::TRANSFORM:
		{
			if (!entity.hasComponent<TransformComponent>())
			{
				return false;
			}

			auto& transform = entity.getComponent<TransformComponent>();

			float fl3_pos[3];
			transform.getPosition().getAs3Values(fl3_pos);
			if (ImGui::InputFloat3("Positon", fl3_pos, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				//g_undoEntities.emplace_back(g_currentEntity);
				//WorldManager::Get()->exportEntity(WorldManager::Get()->getEntityByID(g_currentEntity), "undo.prev" + std::to_string(g_undoEntities.size()));

				transform.setPosition(irr::core::vector3df(fl3_pos[0], fl3_pos[1], fl3_pos[2]));
				WorldManager::Get()->renderSystem()->forceTransformUpdate();
			}
			float fl3_rot[3];
			transform.getRotation().getAs3Values(fl3_rot);
			if (ImGui::InputFloat3("Rotation", fl3_rot, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				//g_undoEntities.emplace_back(g_currentEntity);
				//WorldManager::Get()->exportEntity(WorldManager::Get()->getEntityByID(g_currentEntity), "undo.prev" + std::to_string(g_undoEntities.size()));

				transform.setRotation(irr::core::vector3df(fl3_rot[0], fl3_rot[1], fl3_rot[2]));
				WorldManager::Get()->renderSystem()->forceTransformUpdate();
			}
			float fl3_scl[3];
			transform.getScale().getAs3Values(fl3_scl);
			if (ImGui::InputFloat3("Scale", fl3_scl, 2, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				//g_undoEntities.emplace_back(g_currentEntity);
				//WorldManager::Get()->exportEntity(WorldManager::Get()->getEntityByID(g_currentEntity), "undo.prev" + std::to_string(g_undoEntities.size()));

				transform.setScale(irr::core::vector3df(fl3_scl[0], fl3_scl[1], fl3_scl[2]));
				WorldManager::Get()->renderSystem()->forceTransformUpdate();
			}

			ImGui::Spacing();

			if (transform.isChild)
			{
				auto &parent_ent = WorldManager::Get()->managerSystem()->getEntityByID(transform.parent);
				if (parent_ent.isValid())
				{
					ImGui::Text("--- Parent ---");
					ImGui::Spacing();

					auto pname = parent_ent.getComponent<DescriptorComponent>().name;
					auto pid = parent_ent.getComponent<DescriptorComponent>().id;

					ImGui::Text("Parent Name: %s", pname);
					ImGui::Text("Parent ID  : %i", pid);
				}
				else
				{
					ImGui::Text("Parent entity is not valid!");
				}
			}

			if (transform.isParent && transform.isChild)
			{
				ImGui::Spacing();
				ImGui::Spacing();
			}

			if (transform.isParent)
			{
				if (!transform.children.empty())
				{
					ImGui::Text("--- %i Children ---", transform.children.size());

					ImGui::Spacing();

					for (auto child_id : transform.children)
					{
						auto &child_ent = WorldManager::Get()->managerSystem()->getEntityByID(child_id);
						if (child_ent.isValid())
						{
							auto pname = child_ent.getComponent<DescriptorComponent>().name;
							auto pid   = child_ent.getComponent<DescriptorComponent>().id;

							ImGui::Text("   Child Name: %s", pname);
							ImGui::Text("   Child ID  : %i", pid);
						}
						else
						{
							ImGui::Text("   Child entity is not valid!");
						}

						ImGui::Spacing();
					}
				}

				static char buf[8];
				ImGui::InputText("Add Child by ID", buf, 8, ImGuiInputTextFlags_CharsDecimal);
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					auto &new_child = WorldManager::Get()->managerSystem()->getEntityByID(std::stoi(std::string(buf)));
					memset(buf, 0, 8);

					if (new_child.isValid())
					{
						if (new_child.hasComponent<TransformComponent>())
						{
							transform.addChild(new_child.getComponent<TransformComponent>().node);
						}
					}
				}
			}
					

			break;
		}
	case ENTITY_COMPONENT::TRIGGERZONE:
		{
			if (!entity.hasComponent<TriggerZoneComponent>())
			{
				return false;
			}

			auto& zone = entity.getComponent<TriggerZoneComponent>();

			{
				char buf[256];
				memset(buf, 0, 256);
				for (auto i = 0U; i < zone.entity.size() && i < 256; i++)
				{
					buf[i] = zone.entity[i];
				}

				if (ImGui::InputText("Detect Entity", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					zone.entity = buf;
				}
			}
			{
				char buf[256];
				memset(buf, 0, 256);
				for (auto i = 0U; i < zone.triggered_entity.size() && i < 256; i++)
				{
					buf[i] = zone.triggered_entity[i];
				}

				if (ImGui::InputText("Trigger Entity", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					zone.triggered_entity = buf;
				}
			}

			int mtype = static_cast<unsigned int>(zone.mask);
			ImGui::PushID(1);
			if (ImGui::InputInt("", &mtype, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (mtype > 2)
				{
					mtype = 0;
				}
				if (mtype < 0)
				{
					mtype = 0;
				}

				zone.mask = static_cast<TRIGGER_ZONE_MASK>(mtype);
			}
			ImGui::PopID();

			std::string mask_type;

			switch (zone.mask)
			{
			case TRIGGER_ZONE_MASK::NONE:
				mask_type = "NONE";
				break;
			case TRIGGER_ZONE_MASK::PLAYER_ONLY:
				mask_type = "PLAYER_ONLY";
				break;
			case TRIGGER_ZONE_MASK::ENTITY_NAME:
				mask_type = "ENTITY_NAME";
				break;
			default:
				mask_type = "INVALID";
				break;
			}
			ImGui::SameLine();
			ImGui::Text("Mask: %s", mask_type.c_str());

			ImGui::Checkbox("Single Use", &zone.single_use);

			ImGui::Checkbox("Toggle", &zone.toggle);

			ImGui::Checkbox("Invert Output", &zone.invert);

			break;
		}
	default:
		{
			return false;
		}
	}

	return true;
}

void EditorInterface::add_component(ENTITY_COMPONENT component, anax::Entity& entity)
{
	switch (component)
	{
	case ENTITY_COMPONENT::AUTOKILL:
		{
			entity.addComponent<AutoKillComponent>();
			break;
		}
	case ENTITY_COMPONENT::BILLBOARDSPRITE:
		{
			entity.addComponent<BillboardSpriteComponent>();
			break;
		}
	case ENTITY_COMPONENT::CAMERA:
		{
			entity.addComponent<CameraComponent>();
			break;
		}
	case ENTITY_COMPONENT::CHARACTERCONTROLLER:
		{
			entity.addComponent<CCTComponent>();
			break;
		}
	case ENTITY_COMPONENT::DAMAGERECEIVER:
		{
			entity.addComponent<DamageReceiverComponent>();
			break;
		}
	case ENTITY_COMPONENT::DATA:
		{
			entity.addComponent<DataComponent>();
			break;
		}
	case ENTITY_COMPONENT::DEBUGMESH:
		{
			entity.addComponent<DebugMeshComponent>();
			break;
		}
	case ENTITY_COMPONENT::DEBUGSPRITE:
		{
			entity.addComponent<DebugSpriteComponent>();
			break;
		}
	case ENTITY_COMPONENT::DESCRIPTOR:
		{
			entity.addComponent<DescriptorComponent>();
			break;
		}
	case ENTITY_COMPONENT::INTERACTION:
		{
			entity.addComponent<InteractionComponent>();
			break;
		}
	case ENTITY_COMPONENT::ITEM:
		{
			entity.addComponent<ItemComponent>();
			break;
		}
	case ENTITY_COMPONENT::LIGHT:
		{
			entity.addComponent<LightComponent>();
			break;
		}
	case ENTITY_COMPONENT::LOGIC:
		{
			entity.addComponent<LogicComponent>();
			break;
		}
	case ENTITY_COMPONENT::MARKER:
		{
			entity.addComponent<MarkerComponent>();
			break;
		}
	case ENTITY_COMPONENT::MESH:
		{
			entity.addComponent<MeshComponent>();
			break;
		}
	case ENTITY_COMPONENT::NPC:
		{
			entity.addComponent<NPCComponent>();
			break;
		}
	case ENTITY_COMPONENT::PHYSICS:
		{
			entity.addComponent<PhysicsComponent>();
			break;
		}
	case ENTITY_COMPONENT::PREFAB:
		{
			entity.addComponent<PrefabComponent>();
			break;
		}
	case ENTITY_COMPONENT::RENDER:
		{
			entity.addComponent<RenderComponent>();
			break;
		}
	case ENTITY_COMPONENT::SCRIPT:
		{
			entity.addComponent<ScriptComponent>();
			break;
		}
	case ENTITY_COMPONENT::SOUND:
		{
			entity.addComponent<SoundComponent>();
			break;
		}
	case ENTITY_COMPONENT::SOUNDLISTENER:
		{
			entity.addComponent<SoundListenerComponent>();
			break;
		}
	case ENTITY_COMPONENT::TRANSFORM:
		{
			entity.addComponent<TransformComponent>();
			break;
		}
	case ENTITY_COMPONENT::TRIGGERZONE:
		{
			entity.addComponent<TriggerZoneComponent>();
			break;
		}
	}
}

bool EditorInterface::has_component(ENTITY_COMPONENT component, anax::Entity& entity)
{
	switch (component)
	{
	case ENTITY_COMPONENT::AUTOKILL:
		{
			return entity.hasComponent<AutoKillComponent>();
		}
	case ENTITY_COMPONENT::BILLBOARDSPRITE:
		{
			return entity.hasComponent<BillboardSpriteComponent>();
		}
	case ENTITY_COMPONENT::CAMERA:
		{
			return entity.hasComponent<CameraComponent>();
		}
	case ENTITY_COMPONENT::CHARACTERCONTROLLER:
		{
			return entity.hasComponent<CCTComponent>();
		}
	case ENTITY_COMPONENT::DAMAGERECEIVER:
		{
			return entity.hasComponent<DamageReceiverComponent>();
		}
	case ENTITY_COMPONENT::DATA:
		{
			return entity.hasComponent<DataComponent>();
		}
	case ENTITY_COMPONENT::DEBUGMESH:
		{
			return entity.hasComponent<DebugMeshComponent>();
		}
	case ENTITY_COMPONENT::DEBUGSPRITE:
		{
			return entity.hasComponent<DebugSpriteComponent>();
		}
	case ENTITY_COMPONENT::DESCRIPTOR:
		{
			return entity.hasComponent<DescriptorComponent>();
		}
	case ENTITY_COMPONENT::INTERACTION:
		{
			return entity.hasComponent<InteractionComponent>();
		}
	case ENTITY_COMPONENT::ITEM:
		{
			return entity.hasComponent<ItemComponent>();
		}
	case ENTITY_COMPONENT::LIGHT:
		{
			return entity.hasComponent<LightComponent>();
		}
	case ENTITY_COMPONENT::LOGIC:
		{
			return entity.hasComponent<LogicComponent>();
		}
	case ENTITY_COMPONENT::MARKER:
		{
			return entity.hasComponent<MarkerComponent>();
		}
	case ENTITY_COMPONENT::MESH:
		{
			return entity.hasComponent<MeshComponent>();
		}
	case ENTITY_COMPONENT::NPC:
		{
			return entity.hasComponent<NPCComponent>();
		}
	case ENTITY_COMPONENT::PHYSICS:
		{
			return entity.hasComponent<PhysicsComponent>();
		}
	case ENTITY_COMPONENT::PREFAB:
		{
			return entity.hasComponent<PrefabComponent>();
		}
	case ENTITY_COMPONENT::RENDER:
		{
			return entity.hasComponent<RenderComponent>();
		}
	case ENTITY_COMPONENT::SCRIPT:
		{
			return entity.hasComponent<ScriptComponent>();
		}
	case ENTITY_COMPONENT::SOUND:
		{
			return entity.hasComponent<SoundComponent>();
		}
	case ENTITY_COMPONENT::SOUNDLISTENER:
		{
			return entity.hasComponent<SoundListenerComponent>();
		}
	case ENTITY_COMPONENT::TRANSFORM:
		{
			return entity.hasComponent<TransformComponent>();
		}
	case ENTITY_COMPONENT::TRIGGERZONE:
		{
			return entity.hasComponent<TriggerZoneComponent>();
		}
	default:
		return false;
	}
}

//#define ALL_TREES_OPEN_DEFAULT
void EditorInterface::draw_window_entity_debug_info()
{
	if (!m_windowData.draw_window_entity_debug_info) { return; }

	ImGui::SetNextWindowSize(DPI_SCALED_IMVEC2(250, 350));
	if (ImGui::Begin("Entity Debugging Information", &m_windowData.draw_window_entity_debug_info))
	{
		if (g_currentSelectedObjectType == static_cast<unsigned int>(SELECTED_OBJECT_TYPE::ENTITY) && g_currentEntity < _entity_null_value)
		{
			auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(g_currentEntity);

			if (entity.isValid())
			{
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<DescriptorComponent>() && ImGui::CollapsingHeader("Descriptor"))
				{
					ImGui::Text("Entity ID: %i", entity.getComponent<DescriptorComponent>().id);
					ImGui::Text("Entity Name: %s", entity.getComponent<DescriptorComponent>().name);
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<TransformComponent>() && ImGui::CollapsingHeader("Transform"))
				{
					ImGui::Text("Node ID: %i", entity.getComponent<TransformComponent>().node->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<DataComponent>() && ImGui::CollapsingHeader("Data"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<RenderComponent>() && ImGui::CollapsingHeader("Render"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<MeshComponent>() && ImGui::CollapsingHeader("Mesh"))
				{
					ImGui::Text("Mesh Node ID: %i", entity.getComponent<MeshComponent>().node->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<CameraComponent>() && ImGui::CollapsingHeader("Camera"))
				{
					ImGui::Text("Camera Node ID: %i", entity.getComponent<CameraComponent>().camera->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<LightComponent>() && ImGui::CollapsingHeader("Light"))
				{
					ImGui::Text("Light Node ID: %i", entity.getComponent<LightComponent>().node->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<PhysicsComponent>() && ImGui::CollapsingHeader("Physics"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<CCTComponent>() && ImGui::CollapsingHeader("CCT"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<BillboardSpriteComponent>() && ImGui::CollapsingHeader("Billboard Sprite"))
				{
					ImGui::Text("Mesh ID: %i", entity.getComponent<BillboardSpriteComponent>().node->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<DebugSpriteComponent>() && ImGui::CollapsingHeader("DebugSprite"))
				{
					ImGui::Text("Mesh Node ID: %i", entity.getComponent<DebugSpriteComponent>().node->getID());
					ImGui::Text("Selector Node ID: %i", entity.getComponent<DebugSpriteComponent>().selectorNode->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<DebugMeshComponent>() && ImGui::CollapsingHeader("DebugMesh"))
				{
					ImGui::Text("Mesh Node ID: %i", entity.getComponent<DebugMeshComponent>().node->getID());
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<ScriptComponent>() && ImGui::CollapsingHeader("Script"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<LogicComponent>() && ImGui::CollapsingHeader("Logic Event"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<SoundComponent>() && ImGui::CollapsingHeader("Sound"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<MarkerComponent>() && ImGui::CollapsingHeader("Marker"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<TriggerZoneComponent>() && ImGui::CollapsingHeader("Trigger Zone"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<PrefabComponent>() && ImGui::CollapsingHeader("Prefab"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<NPCComponent>() && ImGui::CollapsingHeader("NPC"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<AutoKillComponent>() && ImGui::CollapsingHeader("Auto Kill"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<DamageReceiverComponent>() && ImGui::CollapsingHeader("Damage Receiver"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<ItemComponent>() && ImGui::CollapsingHeader("Item"))
				{
					ImGui::Text("No Data Present");
				}
#ifdef ALL_TREES_OPEN_DEFAULT
				ImGui::SetNextTreeNodeOpened(true);
#endif
				if (entity.hasComponent<SoundListenerComponent>() && ImGui::CollapsingHeader("Sound Listener"))
				{
					ImGui::Text("No Data Present");
				}
			}
			else
			{
				ImGui::Text("No valid entity selected.");
			}
		}
	}
	ImGui::End();
}

