#include "Bindings.h"

#include "Engine/Engine.h"

#include "ScriptExceptions.h"

// CAMERA
irr::core::vector3df AS_Get_Camera_Position()
{
    return RenderManager::Get()->sceneManager()->getActiveCamera()->getAbsoluteTransformation().getTranslation();
}
irr::core::vector3df AS_Get_Camera_Rotation()
{
	return RenderManager::Get()->sceneManager()->getActiveCamera()->getRotation();
}
irr::core::vector3df AS_Get_Camera_Look_Dir()
{
    return Math::GetDirectionVector(RenderManager::Get()->sceneManager()->getActiveCamera()->getRotation(), false);
}
irr::core::vector3df AS_Get_Camera_Look_Dir_Norm()
{
	return Math::GetDirectionVector(RenderManager::Get()->sceneManager()->getActiveCamera()->getRotation(), true);
}

// ANIMATION
void AS_SetFrame_id(entityid id, float frame)
{
	WorldManager::Get()->renderSystem()->setFrame(id, frame);
}
void AS_SetFrameLoop_id(entityid id, float begin, float end)
{
	WorldManager::Get()->renderSystem()->setFrameLoop(id, begin, end);
}
void AS_PlayAnimation(entityid id, std::string animation)
{
	WorldManager::Get()->renderSystem()->playAnimation(id, animation);
}
void AS_LoopAnimation(entityid id, std::string animation)
{
	WorldManager::Get()->renderSystem()->loopAnimation(id, animation);
}

bool AS_IsAnimating(entityid e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return false;
	}

	if (entity.hasComponent<MeshComponent>()) 
	{
		if (entity.getComponent<MeshComponent>().animation_call_back) 
		{
			return entity.getComponent<MeshComponent>().animation_call_back->hasAnimationEnded();
		}

		return false;
	}

	return false;
}

static std::string AS_LastPlayedAnim(entityid e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return std::string();
	}

	if (entity.hasComponent<MeshComponent>()) 
	{
		return entity.getComponent<MeshComponent>().lastPlayedAnimation.name;
	}

	return std::string();
}

// RENDER
void AS_SetEntityRenderVisible(std::string e, bool b)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByName(e);
	
	if (!entity.isValid())
	{
		return;
	}

	if (entity.hasComponent<RenderComponent>())
	{
		WorldManager::Get()->managerSystem()->getEntityByName(e).getComponent<RenderComponent>().isVisible = b;
	}
}

void AS_SetEntityRenderVisibleID(entityid e, bool b)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return;
	}
	
	if (entity.hasComponent<RenderComponent>())
	{
		WorldManager::Get()->managerSystem()->getEntityByID(e).getComponent<RenderComponent>().isVisible = b;
	}
}

bool AS_GetEntityRenderVisibleID(entityid e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return false;
	}
	
	if (entity.hasComponent<RenderComponent>())
	{
		return WorldManager::Get()->managerSystem()->getEntityByID(e).getComponent<RenderComponent>().isVisible;
	}

	return false;
}

irr::core::vector3df AS_GetBoundingBoxExtent(entityid e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return irr::core::vector3df();
	}
	
	if (entity.hasComponent<MeshComponent>()) 
	{
		return entity.getComponent<MeshComponent>().node->getTransformedBoundingBox().getExtent();
	}

	return irr::core::vector3df();
}

void AS_SwapTexture(entityid e, int tid)
{
	WorldManager::Get()->renderSystem()->swapTexture(e, tid);
}

void AS_SetVisibleLightComp(entityid e, bool b)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return;
	}

	if (entity.hasComponent<LightComponent>()) 
	{
		entity.getComponent<LightComponent>().visible = b;
	}
}

bool AS_IsVisibleLightComp(entityid e)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(e);

	if (!entity.isValid())
	{
		return false;
	}

	if (entity.hasComponent<LightComponent>())
	{
		return entity.getComponent<LightComponent>().visible;
	}

	return false;
}

// RAYCAST
RaycastResultData g_raycastData;
irr::core::vector3df g_raycastStartPoint;

bool AS_Raycast(irr::core::vector3df start, irr::core::vector3df end)
{
	g_raycastData = RenderManager::Get()->raycastWorldPosition(start, end, true);
	g_raycastStartPoint = start;
	return g_raycastData.hit;
}

bool AS_Raycast_Camera()
{
	if (RenderManager::Get()->sceneManager()->getActiveCamera())
	{
		g_raycastData = RenderManager::Get()->raycastWorldPosition(
			RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition(), RenderManager::Get()->sceneManager()->getActiveCamera()->getTarget(), true);
		
		g_raycastStartPoint = RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition();
		
		return g_raycastData.hit;
	}

	return false;
}

bool AS_Raycast_Camera_Range(float range = 0.0f)
{
	if (RenderManager::Get()->sceneManager()->getActiveCamera())
	{
		g_raycastData = RenderManager::Get()->raycastWorldPosition(
			RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition(), RenderManager::Get()->sceneManager()->getActiveCamera()->getTarget(), true);

		g_raycastStartPoint = RenderManager::Get()->sceneManager()->getActiveCamera()->getPosition();

		return g_raycastData.hit;
	}

	return false;
}

bool AS_Raycast_Last_Hit()
{
	return g_raycastData.hit;
}

irr::core::vector3df AS_Raycast_Last_Point()
{
	return g_raycastData.point;
}

int AS_Raycast_Last_EID()
{
	if (g_raycastData.node) {
		return g_raycastData.node->getID();
	}

	return 0xFFFF;
}

float AS_Raycast_Last_Length()
{
	return hypot(
		hypot(
			g_raycastStartPoint.X - g_raycastData.point.X,
			g_raycastStartPoint.Y - g_raycastData.point.Y),
		g_raycastStartPoint.Z - g_raycastData.point.Z);
}

static std::string as_getMaterial(irr::core::vector3df s, irr::core::vector3df e)
{
	return Engine::Get()->getMaterialBuilder().getMaterialName(Engine::Get()->getMaterialBuilder().getMaterialFromTexture(RenderManager::Get()->getMeshMaterialFromRay(s, e)));
}

void ScriptBindings::RegisterRender(asIScriptEngine* engine)
{
    engine->SetDefaultNamespace("render");
    {
        engine->RegisterGlobalFunction("vector3d getCameraPosition()", asFUNCTION(AS_Get_Camera_Position), asCALL_CDECL);
		engine->RegisterGlobalFunction("vector3d getCameraRotation()", asFUNCTION(AS_Get_Camera_Rotation), asCALL_CDECL);
        engine->RegisterGlobalFunction("vector3d getCameraLookDir()", asFUNCTION(AS_Get_Camera_Look_Dir), asCALL_CDECL);
        engine->RegisterGlobalFunction("vector3d getCameraLookDirNormalized()", asFUNCTION(AS_Get_Camera_Look_Dir_Norm), asCALL_CDECL);

		engine->RegisterGlobalFunction("void visible(string, bool)", asFUNCTION(AS_SetEntityRenderVisible), asCALL_CDECL);
		engine->RegisterGlobalFunction("void visible(int, bool)", asFUNCTION(AS_SetEntityRenderVisibleID), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool visible(int)", asFUNCTION(AS_GetEntityRenderVisibleID), asCALL_CDECL);

		engine->RegisterGlobalFunction("vector3d getBoundingBox(int)", asFUNCTION(AS_GetBoundingBoxExtent), asCALL_CDECL);

		engine->RegisterGlobalFunction("void swapTexture(int, int)", asFUNCTION(AS_SwapTexture), asCALL_CDECL);

		engine->RegisterGlobalFunction("void setLightComponentVisible(int, bool)", asFUNCTION(AS_SetVisibleLightComp), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool isLightComponentVisible(int)", asFUNCTION(AS_IsVisibleLightComp), asCALL_CDECL);

		engine->RegisterGlobalFunction("string getMaterial(vector3d, vector3d)", asFUNCTION(as_getMaterial), asCALL_CDECL);
    }

	engine->SetDefaultNamespace("animation");
	{
		engine->RegisterGlobalFunction("void set(int, float)", asFUNCTION(AS_SetFrame_id), asCALL_CDECL);
		engine->RegisterGlobalFunction("void play(int, string)", asFUNCTION(AS_PlayAnimation), asCALL_CDECL);
		engine->RegisterGlobalFunction("void loop(int, float, float)", asFUNCTION(AS_SetFrameLoop_id), asCALL_CDECL);
		engine->RegisterGlobalFunction("void loop(int, string)", asFUNCTION(AS_LoopAnimation), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool ended(int)", asFUNCTION(AS_IsAnimating), asCALL_CDECL);
		engine->RegisterGlobalFunction("string last(int)", asFUNCTION(AS_LastPlayedAnim), asCALL_CDECL);
	}

	engine->SetDefaultNamespace("raycast");
	{
		engine->RegisterGlobalFunction("bool line(vector3d, vector3d)", asFUNCTION(AS_Raycast), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool camera()", asFUNCTION(AS_Raycast_Camera), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool camera(float)", asFUNCTION(AS_Raycast_Camera_Range), asCALL_CDECL);
		engine->RegisterGlobalFunction("bool test()", asFUNCTION(AS_Raycast_Last_Hit), asCALL_CDECL);
		engine->RegisterGlobalFunction("vector3d position()", asFUNCTION(AS_Raycast_Last_Point), asCALL_CDECL);
		engine->RegisterGlobalFunction("int id()", asFUNCTION(AS_Raycast_Last_EID), asCALL_CDECL);
		engine->RegisterGlobalFunction("float length()", asFUNCTION(AS_Raycast_Last_Length), asCALL_CDECL);
	}

    engine->SetDefaultNamespace("");
}