#include "CameraSystem.h"

#include "Engine/Engine.h"
#include "Utility/Utility.h"

using namespace anax;
using namespace irr;
using namespace core;

void CameraSystem::onEntityAdded(Entity& entity)
{
    auto& cameraComponent = entity.getComponent<CameraComponent>();
    auto& transformComponent = entity.getComponent<TransformComponent>();

    cameraComponent.camera = RenderManager::Get()->sceneManager()->addCameraSceneNode();
	cameraComponent.camera->setID(entity.getComponent<DescriptorComponent>().id);

    RenderManager::Get()->sceneManager()->setActiveCamera(cameraComponent.camera);

    cameraComponent.camera->setAspectRatio(static_cast<float>(RenderManager::Get()->getConfiguration().width) / static_cast<float>(RenderManager::Get()->getConfiguration().height));
    
    cameraComponent.camera->setNearValue(CAMERA_NEAR);
    cameraComponent.camera->setFarValue(CAMERA_FAR);

    cameraComponent.camera->setPosition(vector3df(0, 0, 0));
    cameraComponent.camera->setRotation(vector3df(0, 0, 0));

    cameraComponent.targetNode = RenderManager::Get()->sceneManager()->addEmptySceneNode();
    cameraComponent.targetNode->setPosition(cameraComponent.target);
    cameraComponent.targetNode->setParent(cameraComponent.camera);

    cameraComponent.nearTargetNode = RenderManager::Get()->sceneManager()->addEmptySceneNode();
    cameraComponent.nearTargetNode->setPosition(cameraComponent.neartarget);
    cameraComponent.nearTargetNode->setParent(cameraComponent.camera);
	
    cameraComponent.camera->setPosition(transformComponent.position);
    cameraComponent.camera->setRotation(transformComponent.rotation);

    RenderManager::Get()->sceneManager()->setActiveCamera(cameraComponent.camera);

	cameraComponent.camera->updateAbsolutePosition();

	// Finish initializing the camera node after all components have been processed, see update()
}

void CameraSystem::onEntityRemoved(Entity& entity)
{
    auto& cameraComponent = entity.getComponent<CameraComponent>();

    RenderManager::Get()->sceneManager()->addToDeletionQueue(cameraComponent.camera);
    RenderManager::Get()->sceneManager()->addToDeletionQueue(cameraComponent.targetNode);
    RenderManager::Get()->sceneManager()->addToDeletionQueue(cameraComponent.nearTargetNode);
}

void CameraSystem::update()
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto& cameraComponent = entity.getComponent<CameraComponent>();
        auto& transformComponent = entity.getComponent<TransformComponent>();

        auto& camera = cameraComponent.camera;
        auto& target = cameraComponent.targetNode;
        auto& nearTarget = cameraComponent.nearTargetNode;

		// DEBUG: Finish initializing the camera, addChild() cause crash with nullptr exception ???
		/*if (!cameraComponent.hasInit)
		{
			transformComponent.addChild(cameraComponent.camera);

			cameraComponent.camera->setPosition(cameraComponent.offset);

			cameraComponent.hasInit = true;
		}*/

		/*if (!cameraComponent.hasInit)
		{
			transformComponent.node->addChild(cameraComponent.camera);
			cameraComponent.hasInit = true;
		}*/

        camera->setPosition(
            transformComponent.position + cameraComponent.offset);
        camera->setRotation(transformComponent.rotation);

        camera->updateAbsolutePosition();
        target->updateAbsolutePosition();
        nearTarget->updateAbsolutePosition();

        camera->setTarget(target->getAbsolutePosition());
		
        cameraComponent.lookat = Math::GetDirectionVector(camera->getRotation());
    }
}
