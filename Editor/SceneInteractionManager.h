#pragma once

#include <memory>

#include <irrlicht.h>

#include "anax/Entity.hpp"

#include "Engine/World/Components/TransformComponent.h"
#include "Engine/Interface/ImTransformControl.h"

#define CLIPBOARD_ENTITY_FILENAME "-cent"

extern unsigned int g_currentEntity;
extern unsigned int g_currentMesh;
extern unsigned int g_currentPrefab;
extern std::string g_currentScene;
extern std::string g_currentSelectedTexture;
extern unsigned int g_currentSelectedObjectType;

enum class SELECTED_OBJECT_TYPE
{
    NONE,
    ENTITY,
    MESH,
    PREFAB
};

enum class TRANSFORM_WIDGET_MODE
{
    TRANSLATE,
    ROTATE,
    SCALE,
    LOCAL,
    WORLD
};

struct EditorConfiguration
{
	bool
		drawPointLightBounds,
		useSnap;

	float snapX, snapY, snapZ;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(
			CEREAL_NVP(drawPointLightBounds),
			CEREAL_NVP(useSnap),
			CEREAL_NVP(snapX),
			CEREAL_NVP(snapY),
			CEREAL_NVP(snapZ));
	}

	EditorConfiguration() :
		drawPointLightBounds(false),
		useSnap(false),
		snapX(0.25f),
		snapY(0.25f),
		snapZ(0.25f)
	{}
};

class SceneInteractionManager
{
public:
	void init();
	void update();
	void draw();
	void destroy();

	void setSelectedEntity(unsigned int ent)
	{
        m_selectedEntities.clear();
        std::vector<entityid>().swap(m_selectedEntities);
	    m_selectedEntities.emplace_back(ent);
		g_currentEntity = ent;
	}

    void addAnotherSelectedEntity(unsigned int ent)
    {
        m_selectedEntities.emplace_back(ent);
    }

    void clearSelectedEntities()
    {
        m_selectedEntities.clear();
        std::vector<entityid>().swap(m_selectedEntities);
    }

	unsigned int getSelectedEntity()
	{
	    if (!m_selectedEntities.empty()) 
            return m_selectedEntities.at(0);

        return _entity_null_value;
	}

    void deleteEntity();
    void cutEntity();
    void copyEntity();
    void pasteEntity();

    void setTransformWidgetMode(TRANSFORM_WIDGET_MODE mode);

    void useSnap(bool snap)
    {
        m_useSnap = snap;
    }
    bool isSnap()
    {
        return m_useSnap;
    }

    void setSnap(float snap)
    {
		m_configuration.snapX = snap;
		m_configuration.snapY = snap;
		m_configuration.snapZ = snap;
		saveConfiguration(m_configuration);
    }
    float getSnapUnit()
    {
        return m_snap[0];
    }

    ImTransformControl::OPERATION getWidgetToolMode() { return m_widgetType; }
    ImTransformControl::MODE getWidgetCoordMode() { return m_widgetCoordSet; }
    std::string getWidgetToolModeStr() { return m_selectedWidgetType; }
    std::string getWidgetCoordModeStr() { return m_selectedTransType; }

    bool isEntitySelected() {
        if (!m_selectedEntities.empty()) {
            return true;
        }

        return false;
    }
    bool multipleEntitiesSelected() { return m_selectedEntities.size() > 1; }

    unsigned int getCurrentSelectedObject()
    {
        return m_currentSelectedObject;
    }

	EditorConfiguration getConfiguration() const { return m_configuration; }
	void saveConfiguration(EditorConfiguration& configuration);

	void selectNewSpawnedEntityNextFrame() { m_selectNewSpawnedEntity = true; }

private:
    std::vector<entityid> m_selectedEntities;

    bool m_isWidgetDrawn, m_useSnap, m_selectNewSpawnedEntity;

    float m_snap[3];

    entityid m_selectedEntity;

    std::string m_selectedWidgetType, m_selectedTransType;

    ImTransformControl::OPERATION m_widgetType;
    ImTransformControl::MODE m_widgetCoordSet;

    unsigned int m_currentSelectedObject;

	EditorConfiguration m_configuration;

};
