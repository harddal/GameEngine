#pragma once

#include "anax/anax.hpp"
#include "Engine/World/Components.h"

#include <irrlicht.h>

class AnimationCallback : public irr::scene::IAnimationEndCallBack
{
public:
    // Can only call once per frame per mesh since the state (isEnded) resets when the function is executed
	bool hasAnimationEnded()
	{
		auto b = isEnded;
		isEnded = false;
		return b;
	}

protected:
	void OnAnimationEnd(irr::scene::IAnimatedMeshSceneNode* node) override
	{
		isEnded = true;
	}

	bool isEnded = true;
};

class GameEngine;

class RenderSystem
    : public anax::System<anax::Requires<DescriptorComponent, TransformComponent, RenderComponent>>
{
public:
    void onEntityAdded(anax::Entity& entity) override;
    void onEntityRemoved(anax::Entity& entity) override;
	 
    void update();

    void setMeshComponentData(anax::Entity& entity);
    void setDebugMeshComponentData(anax::Entity& entity);
    void setLightComponentData(anax::Entity& entity);
    void setBillboardComponentData(anax::Entity& entity);
    void setDebugSpriteComponentData(anax::Entity& entity);

	void forceTransformUpdate(bool updateLights = true);

    void remove(std::string name);

    void setVisible(std::string name, bool visible);

	void setFrame(entityid id, irr::s32 frame);
	void setFrameLoop(entityid id, irr::s32 begin, irr::s32 end);
    
    void setFrame(std::string name, irr::s32 frame);
    void setFrameLoop(std::string name, irr::s32 begin, irr::s32 end);

	void playAnimation(entityid id, std::string animation);
    void loopAnimation(entityid id, std::string animation);
	void playAnimation(const anax::Entity& entity, std::string animation);

	bool swapMesh(entityid id, std::string file);
    void setNodeMaterialType(entityid id, irr::video::E_MATERIAL_TYPE material_type);
    
	void setLightData(LightComponent& light)
	{
		light.node->setLightType(static_cast<irr::video::E_LIGHT_TYPE>(light.type));
		light.data = light.node->getLightData();
		light.data.OuterCone = light.outerCone;
		light.data.InnerCone = light.innerCone;
		light.data.Falloff = light.falloff;
		light.data.DiffuseColor = light.color_diffuse;
		light.node->setLightData(light.data);
		light.node->setRadius(light.radius);
		light.node->setVisible(light.visible);
	}

    void setDebugSpriteVisible(bool visible = true)
	{
		m_drawDebugSprites = visible;
		m_applyDebugSpriteVisSetting = true;
	}
	bool isDebugSpriteVisible()
	{
		return m_drawDebugSprites;
	}

	bool getEntityAABBIntersection(entityid e1, entityid e2);

	void swapTexture(entityid id, unsigned int texture_id);

private:
	bool m_drawDebugSprites = false, m_applyDebugSpriteVisSetting = false;
};
