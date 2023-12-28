#include <fstream>

#include <cereal/archives/xml.hpp>
#include <spdlog/spdlog.h>

#include "Editor/SceneInteractionManager.h"
#include "Editor/Interface/EditorInterface.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Input/InputMap.h"
#include "Engine/Renderer/RenderManager.h"
#include "Engine/World/WorldManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

#define _selected_entity (m_selectedEntities.empty() ? (_entity_null_value) : (m_selectedEntities.at(0)))

unsigned int g_currentEntity = _entity_null_value, g_undoEntity = _entity_null_value, g_undoCount = 0, g_currentMesh =
	             _entity_null_value, g_currentPrefab = _entity_null_value;
std::string g_currentScene;
extern std::string g_currentSelectedTexture = "null";
unsigned int g_currentSelectedObjectType = static_cast<unsigned int>(SELECTED_OBJECT_TYPE::NONE);

static vector3df constrainAngleVector3(vector3df v)
{
	v.X = fmod(v.X, 360);
	if (v.X < 0)
		v.X += 360;

	v.Y = fmod(v.Y, 360);
	if (v.Y < 0)
		v.Y += 360;

	v.Z = fmod(v.Z, 360);
	if (v.Z < 0)
		v.Z += 360;

	return v;
}

void SceneInteractionManager::init()
{
	try
	{
		std::ifstream ifs_editor("config/editor.xml");
		cereal::XMLInputArchive editor_config(ifs_editor);

		editor_config(m_configuration);
	}
	catch (cereal::Exception& ex)
	{
		spdlog::warn("Failed to load editor configuration: {}, default values used", ex.what());

		m_configuration = EditorConfiguration();

		std::ofstream ofs_editor("config/editor.xml");
		cereal::XMLOutputArchive editor_config(ofs_editor);

		editor_config(m_configuration);
	}

	m_selectNewSpawnedEntity = false;

	m_isWidgetDrawn = false;
	m_useSnap = m_configuration.useSnap;

	m_snap[0] = m_configuration.snapX;
	m_snap[1] = m_configuration.snapY;
	m_snap[2] = m_configuration.snapZ;

	m_selectedWidgetType = "Translate";
	m_selectedTransType = "World";

	m_widgetType = ImTransformControl::TRANSLATE;
	m_widgetCoordSet = ImTransformControl::WORLD;

	m_selectedEntities.clear();
	std::vector<entityid>().swap(m_selectedEntities);

	m_currentSelectedObject = static_cast<unsigned int>(SELECTED_OBJECT_TYPE::NONE);
}

void SceneInteractionManager::update()
{
	if (m_selectNewSpawnedEntity)
	{
		setSelectedEntity(WorldManager::Get()->managerSystem()->getMostRecentEntityID());

		m_selectNewSpawnedEntity = false;
	}

	if (InputManager::Get()->isKeyPressed(KEYBOARD_KEY::KEY_ESCAPE))
	{
		//g_currentEntity = _selected_entity;
		clearSelectedEntities();
		//g_currentMesh = _mesh_null_value;
		m_currentSelectedObject = static_cast<unsigned int>(SELECTED_OBJECT_TYPE::NONE);
	}

	static bool mouseClick = false;
	if (InputManager::Get()->getMousePressOnce(0, &mouseClick) && InputManager::Get()->isKeyPressed(
		KEYBOARD_KEY::KEY_LSHIFT))
	{
		// BUG: If entity is deleted, selection will not work due to a bug in the transform control
		// This avoids the bug with a small drawback: don't hold shift and use the transform control at the same time
		//if (!ImTransformControl::IsOver() && !ImTransformControl::IsUsing()) {

		auto node = RenderManager::Get()->getNodeFromCursorPosition();
		if (node)
		{
			// Select entity
			if (node->getID() < _entity_null_value)
			{
				//g_currentMesh = _mesh_null_value;
				setSelectedEntity(node->getID());
				g_currentEntity = _selected_entity;
				g_currentSelectedObjectType = m_currentSelectedObject = static_cast<unsigned int>(
					SELECTED_OBJECT_TYPE::ENTITY);			
			}
			// Select mesh
			/*else if (node->getID() > _entity_null_value && node->getID() < _mesh_null_value) {
                clearSelectedEntities();
                g_currentEntity = _entity_null_value;
                g_currentMesh = _entity_null_value - (_entity_null_value - node->getID());
                g_currentSelectedObjectType = m_currentSelectedObject = static_cast<unsigned int>(SELECTED_OBJECT_TYPE::MESH);
            }*/
		}

		//}
	}
}

void SceneInteractionManager::draw()
{
	m_isWidgetDrawn = false;

	// DEBUG TEMP
	m_useSnap = m_configuration.useSnap;
	m_snap[0] = m_configuration.snapX;
	m_snap[1] = m_configuration.snapY;
	m_snap[2] = m_configuration.snapZ;

	switch (static_cast<SELECTED_OBJECT_TYPE>(m_currentSelectedObject))
	{
	case SELECTED_OBJECT_TYPE::NONE:
	{
	}
	break;
	case SELECTED_OBJECT_TYPE::ENTITY:
	{
		if (_selected_entity < _entity_null_value)
		{
			if (WorldManager::Get()->managerSystem()->getEntityByID(_selected_entity).isValid())
			{
				auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(_selected_entity);

				auto& selectedTransform = entity.getComponent<TransformComponent>();

				m_isWidgetDrawn = true;

				matrix4 transform;
				//transform.setTranslation(selectedTransform.getPosition());
				//transform.setRotationDegrees(selectedTransform.getRotation());
				//transform.setScale(selectedTransform.getScale());

				switch (m_widgetType)
				{
				case ImTransformControl::TRANSLATE:
					transform.setTranslation(selectedTransform.getPosition());
					break;
				case ImTransformControl::ROTATE:
					transform.setTranslation(selectedTransform.getPosition());
					transform.setRotationDegrees(selectedTransform.getRotation());
					break;
				case ImTransformControl::SCALE:
					transform.setTranslation(selectedTransform.getPosition());
					transform.setScale(selectedTransform.getScale());
					break;
				case ImTransformControl::BOUNDS:
					break;
				default:
					break;
				}

				ImGuiIO& io = ImGui::GetIO();

				ImTransformControl::SetDrawlist();

				ImTransformControl::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
				ImTransformControl::Manipulate(
					RenderManager::Get()->sceneManager()->getActiveCamera()->getViewMatrix().pointer(),
					RenderManager::Get()->sceneManager()->getActiveCamera()->getProjectionMatrix().pointer(),
					m_widgetType, m_widgetCoordSet,
					transform.pointer(), nullptr, m_useSnap ? &m_snap[0] : nullptr);

				if (ImTransformControl::IsUsing())
				{
					switch (m_widgetType)
					{
					case ImTransformControl::TRANSLATE:
						selectedTransform.setPosition(transform.getTranslation());
						break;
					case ImTransformControl::ROTATE:
						selectedTransform.setRotation(transform.getRotationDegrees());
						break;
					case ImTransformControl::SCALE:
						selectedTransform.setScale(transform.getScale());
						break;
					case ImTransformControl::BOUNDS:
						break;
					default:
						break;
					}

					WorldManager::Get()->renderSystem()->forceTransformUpdate();
				}

				if (m_configuration.drawPointLightBounds)
				{
					if (entity.hasComponent<LightComponent>())
					{
						auto& light = entity.getComponent<LightComponent>();

						if (light.type == LIGHT_TYPE::LT_POINT)
						{
							if (entity.hasComponent<DebugMeshComponent>())
							{
								entity.getComponent<DebugMeshComponent>().node->setVisible(true);
							}
						}
					}
				}
			}
		}
	}
	break;
	case SELECTED_OBJECT_TYPE::MESH:
	{
		//    if (g_currentMesh < _mesh_null_value) {
		//        auto mesh = _world->getStaticMeshNode(g_currentMesh);

		//        m_isWidgetDrawn = true;

		//        matrix4 transform;
		//        //transform.setTranslation(selectedTransform.getPosition());
		//        //transform.setRotationDegrees(selectedTransform.getRotation());
		//        //transform.setScale(selectedTransform.getScale());

		//        switch (m_widgetType) {
		//        case ImTransformControl::TRANSLATE:
		//            transform.setTranslation(mesh->getPosition());
		//            break;
		//        case ImTransformControl::ROTATE:
		//            transform.setTranslation(mesh->getPosition());
		//            transform.setRotationDegrees(mesh->getRotation());
		//            break;
		//        case ImTransformControl::SCALE:
		//            transform.setTranslation(mesh->getPosition());
		//            transform.setScale(mesh->getScale());
		//            break;
		//        case ImTransformControl::BOUNDS:
		//            break;
		//        default:
		//            break;
		//        }

		//        ImGuiIO& io = ImGui::GetIO();

		//        ImTransformControl::SetDrawlist();

		//        ImTransformControl::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		//        ImTransformControl::Manipulate(
		//            _render->sceneManager()->getActiveCamera()->getViewMatrix().pointer(),
		//            _render->sceneManager()->getActiveCamera()->getProjectionMatrix().pointer(), m_widgetType, m_widgetCoordSet,
		//            transform.pointer(), nullptr, m_useSnap ? &m_snap[0] : nullptr);

		//        if (ImTransformControl::IsUsing()) {
		//            switch (m_widgetType) {
		//            case ImTransformControl::TRANSLATE:
		//                mesh->setPosition(transform.getTranslation());
		//                break;
		//            case ImTransformControl::ROTATE:
		//                mesh->setRotation(transform.getRotationDegrees());
		//                break;
		//            case ImTransformControl::SCALE:
		//                mesh->setScale(transform.getScale());
		//                break;
		//            case ImTransformControl::BOUNDS:
		//                break;
		//            default:
		//                break;
		//            }

		//            _world->renderSystem()->forceTransformUpdate();
		//        }
		//    }
	}
	break;
	}
}

void SceneInteractionManager::destroy()
{
}

void SceneInteractionManager::deleteEntity()
{
	if (_selected_entity < _entity_null_value)
	{
		WorldManager::Get()->killEntityByID(_selected_entity);
		clearSelectedEntities();
		g_currentEntity = _selected_entity;
	}
}

void SceneInteractionManager::cutEntity()
{
	if (_selected_entity < _entity_null_value)
	{
		WorldManager::Get()->exportEntity(
			WorldManager::Get()->managerSystem()->getEntityByID(_selected_entity), CLIPBOARD_ENTITY_FILENAME, true);
		
		WorldManager::Get()->killEntityByID(_selected_entity);
		
		clearSelectedEntities();
		g_currentEntity = _selected_entity;
	}
}

void SceneInteractionManager::copyEntity()
{
	if (_selected_entity < _entity_null_value)
	{
		WorldManager::Get()->exportEntity(
			WorldManager::Get()->managerSystem()->getEntityByID(_selected_entity), CLIPBOARD_ENTITY_FILENAME, true);
	}
}

void SceneInteractionManager::pasteEntity()
{
	WorldManager::Get()->spawnEntity(CLIPBOARD_ENTITY_FILENAME, std::string(), true);
}

void SceneInteractionManager::setTransformWidgetMode(TRANSFORM_WIDGET_MODE mode)
{
	switch (mode)
	{
	case TRANSFORM_WIDGET_MODE::TRANSLATE:
		m_widgetType = ImTransformControl::TRANSLATE;
		m_selectedWidgetType = "Translate";
		break;
	case TRANSFORM_WIDGET_MODE::ROTATE:
		m_widgetType = ImTransformControl::ROTATE;
		m_selectedWidgetType = "Rotate";
		break;
	case TRANSFORM_WIDGET_MODE::SCALE:
		m_widgetType = ImTransformControl::SCALE;
		m_selectedWidgetType = "Scale";
		break;
	case TRANSFORM_WIDGET_MODE::LOCAL:
		m_widgetCoordSet = ImTransformControl::LOCAL;
		m_selectedTransType = "Local";
		break;
	case TRANSFORM_WIDGET_MODE::WORLD:
		m_widgetCoordSet = ImTransformControl::WORLD;
		m_selectedTransType = "World";
		break;
	}
}

void SceneInteractionManager::saveConfiguration(EditorConfiguration& configuration)
{
	std::ofstream ofs_editor("config/editor.xml");
	cereal::XMLOutputArchive editor_config(ofs_editor);

	m_configuration = configuration;
	editor_config(configuration);
}
