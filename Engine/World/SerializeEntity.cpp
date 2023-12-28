#include "WorldManager.h"

#include <string>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tinyxml2.h>

#include "Utility/Utility.h"

#include "Game/GameState.h"

using namespace anax;
using namespace cereal;
using namespace std;
using namespace tinyxml2;

void WorldManager::serializeEntity(Entity& entity, XMLOutputArchive& archive, bool save_transform)
{
	if (entity.hasComponent<DescriptorComponent>())
	{
		if (!entity.getComponent<DescriptorComponent>().isSerializable)
		{
			return;
		}
	}
	else return;
	
    try
    {
        archive.setNextName("entity");
        archive.startNode();

		if (entity.hasComponent<DescriptorComponent>())
		{
			archive.setNextName("descriptor");
			archive.startNode();

			archive(entity.getComponent<DescriptorComponent>());

			archive.finishNode();
		}

		if (entity.hasComponent<TransformComponent>())
		{
			archive.setNextName("transform");
			archive.startNode();

			if (!save_transform)
			{
				entity.getComponent<TransformComponent>().resetTransform();
			}

			archive(entity.getComponent<TransformComponent>());

			archive.finishNode();
		}
    	
        if (entity.hasComponent<BillboardSpriteComponent>())
        {
            archive.setNextName("billboard");
            archive.startNode();

            archive(entity.getComponent<BillboardSpriteComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<CameraComponent>())
        {
            archive.setNextName("camera");
            archive.startNode();

            archive(entity.getComponent<CameraComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<CCTComponent>())
        {
            archive.setNextName("cct");
            archive.startNode();

            archive(entity.getComponent<CCTComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<DebugMeshComponent>())
        {
            archive.setNextName("debugmesh");
            archive.startNode();

            archive(entity.getComponent<DebugMeshComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<DebugSpriteComponent>())
        {
            archive.setNextName("debugsprite");
            archive.startNode();

            archive(entity.getComponent<DebugSpriteComponent>());

            archive.finishNode();
        }
    	
        if (entity.hasComponent<LightComponent>())
        {
            archive.setNextName("light");
            archive.startNode();

            archive(entity.getComponent<LightComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<MeshComponent>())
        {
            archive.setNextName("mesh");
            archive.startNode();

            archive(entity.getComponent<MeshComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<PhysicsComponent>())
        {
            archive.setNextName("physics");
            archive.startNode();

            archive(entity.getComponent<PhysicsComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<RenderComponent>())
        {
            archive.setNextName("render");
            archive.startNode();

            archive(entity.getComponent<RenderComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<ScriptComponent>())
        {
            archive.setNextName("script");
            archive.startNode();

            archive(entity.getComponent<ScriptComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<SoundListenerComponent>())
        {
            archive.setNextName("soundlistener");
            archive.startNode();

            archive(entity.getComponent<SoundListenerComponent>());

            archive.finishNode();
        }

        if (entity.hasComponent<SoundComponent>())
        {
            archive.setNextName("sound");
            archive.startNode();

            archive(entity.getComponent<SoundComponent>());

            archive.finishNode();
        }

        GameState::serializeComponent(entity, archive);

        archive.finishNode();
    }
    catch (Exception& ex)
    {
        spdlog::warn("Failed to serialize entity \'{0}\' {1}",
            entity.isValid() ? entity.getComponent<DescriptorComponent>().name : "NULL", ex.what());
    }
}