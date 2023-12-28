#include "RenderSystem.h"

#include "Engine/Engine.h"
#include "Engine/Resource/FilePaths.h"

#include "Editor/SceneInteractionManager.h"
#include "Editor/EditorState.h"

#define DEBUG_MESH_LIGHT_SPHERE_SCALE 1.5f;

using namespace anax;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

void RenderSystem::setMeshComponentData(Entity& entity)
{
    auto& meshComponent = entity.getComponent<MeshComponent>();

    if (meshComponent.node) {
        meshComponent.node->remove();
    }
    if (meshComponent.selector) {
        meshComponent.selector->drop();
    }

    if (meshComponent.mesh == "_primitive_cube") {
        meshComponent.isPrimitive = true;
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cube.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_sphere") {
        meshComponent.isPrimitive = true;
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/sphere.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_double_tetrahedron") {
        meshComponent.isPrimitive = true;
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/double_tetrahedron.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    // TODO: Check if a valid file path was specified 
    else if (!meshComponent.mesh.empty()) {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh(meshComponent.mesh.c_str());
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else
    {
        meshComponent.mesh = "_primitive_cube";
        meshComponent.isPrimitive = true;
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cube.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }

	if (!meshComponent.node)
	{
		spdlog::error("Failed to load mesh \'{0}\' for entity \'{1}\'", meshComponent.mesh, entity.getComponent<DescriptorComponent>().name);
		// Load a debug mesh instead
		meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/double_tetrahedron.obj");
		meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
		auto* t = RenderManager::Get()->driver()->getTexture("content/texture/color/magenta.png");
		meshComponent.node->setMaterialTexture(0, t);
	}

	//RenderManager::Get()->driver()->addOcclusionQuery(meshComponent.node, meshComponent.trimesh);
	
    // TODO: Implement a way to store textures already loaded to reuse them (a resource manager)
    if (!meshComponent.textures.empty()) {

        auto textures = meshComponent.textures;
        // Use ITexture::getName to give textures names
        auto* t = RenderManager::Get()->driver()->getTexture(textures[0].c_str());
        if (t) {
            //meshComponent.node->setMaterialTexture(0, t);
			meshComponent.node->getMaterial(0).setTexture(0, t);
        }
    }

    meshComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

    auto transform = entity.getComponent<TransformComponent>();


    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        meshComponent.node->setIsDebugObject(true);
    }

    meshComponent.node->setParent(transform.node);

    // DEPRECATED
    //meshComponent.node->setPosition(transform.position);
    //meshComponent.node->setRotation(transform.rotation);
    //meshComponent.node->setScale(transform.scale);

    // No MSAA with deferred rendering
    meshComponent.node->setMaterialFlag(EMF_ANTI_ALIASING, false);

    meshComponent.node->setMaterialFlag(EMF_GOURAUD_SHADING, true);

    meshComponent.node->setMaterialFlag(EMF_LIGHTING, true);
    meshComponent.node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);

    meshComponent.node->setMaterialFlag(EMF_TRILINEAR_FILTER, true);
    meshComponent.node->setMaterialFlag(EMF_ANISOTROPIC_FILTER, true);

    meshComponent.node->setMaterialFlag(EMF_ZBUFFER, !meshComponent.disableZDraw);

	if (meshComponent.transparent)
	{
		//meshComponent.renderMaterial = EMT_TRANSPARENT_ALPHA_CHANNEL;
		meshComponent.node->setMaterialType(meshComponent.renderMaterial);
		meshComponent.node->setMaterialFlag(EMF_ZWRITE_ENABLE, true);

		meshComponent.node->setMaterialFlag(EMF_BLEND_OPERATION, true);
	}

    /* --- DEBUG DRAWING --- */
    //meshComponent.node->setDebugDataVisible(EDS_NORMALS | EDS_BBOX_ALL);

    meshComponent.node->updateAbsolutePosition();

    if (meshComponent.isAnimated) {
        meshComponent.node->setAnimationSpeed(static_cast<irr::f32>(meshComponent.fps));
        meshComponent.node->setLoopMode(false);
        meshComponent.node->setFrameLoop(0, 0);

        meshComponent.animation_call_back = std::make_shared<AnimationCallback>();
        meshComponent.node->setAnimationEndCallback(meshComponent.animation_call_back.get());
    }

    meshComponent.selector = RenderManager::Get()->sceneManager()->createTriangleSelector(meshComponent.node);
    meshComponent.node->setTriangleSelector(meshComponent.selector);

	// Does it do anything? meshComponent.renderMaterial isnt currently used I don't think
  /*  for (u32 i = 0; i < meshComponent.node->getMaterialCount(); i++) {
        meshComponent.node->getMaterial(i).MaterialType = meshComponent.renderMaterial;
    }*/

	//if (!meshComponent.disableDeferredRendering || !meshComponent.transparent)
	{
		RenderManager::Get()->renderer()->getMaterialSwapper()->swapMaterials(meshComponent.node);
	}

	//RenderManager::Get()->effect()->addShadowToNode(meshComponent.node, EFT_NONE, ESM_BOTH);

    meshComponent.node->setVisible(false);
}
void RenderSystem::setDebugMeshComponentData(anax::Entity& entity)
{
    bool isPointer = false;

    auto& meshComponent = entity.getComponent<DebugMeshComponent>();

    if (meshComponent.node) {
        entity.getComponent<DebugMeshComponent>().node->remove();
    }
    if (meshComponent.selector) {
        meshComponent.selector->drop();
    }

    if (meshComponent.mesh == "_primitive_cube") {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cube.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_sphere") {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/sphere_low.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_cylinder") {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cylinder_low.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_double_tetrahedron") {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/double_tetrahedron.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }
    else if (meshComponent.mesh == "_primitive_pointer") {
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cube.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
        meshComponent.node->setScale(vector3df(.01f, .01f, 0.5f));
        isPointer = true;
    }
    else
    {
        meshComponent.mesh = "_primitive_cube";
        meshComponent.trimesh = RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/cube.obj");
        meshComponent.node = RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(meshComponent.trimesh, nullptr, entity.getComponent<DescriptorComponent>().id);
    }

    auto* t = RenderManager::Get()->driver()->getTexture(meshComponent.texture.c_str());
    if (t) {
        meshComponent.node->setMaterialTexture(0, t);
    }

    meshComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

    auto transform = entity.getComponent<TransformComponent>();

	if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode())
	{
		meshComponent.node->setIsDebugObject(true);
	}

    meshComponent.node->setParent(transform.node);
    if (isPointer) {
        meshComponent.node->setPosition(vector3df(0, 0, .5f));
    }

    // DEPRECATED
    //meshComponent.node->setPosition(transform.position);
    //meshComponent.node->setRotation(transform.rotation);
    //meshComponent.node->setScale(transform.scale);

    // No MSAA with deferred rendering
    meshComponent.node->setMaterialFlag(EMF_ANTI_ALIASING, false);

    meshComponent.node->setMaterialFlag(EMF_WIREFRAME, true);

    meshComponent.node->setMaterialFlag(EMF_GOURAUD_SHADING, false);

    meshComponent.node->setMaterialFlag(EMF_LIGHTING, false);
    meshComponent.node->setMaterialFlag(EMF_NORMALIZE_NORMALS, false);

    meshComponent.node->setMaterialFlag(EMF_BILINEAR_FILTER, true);
    meshComponent.node->setMaterialFlag(EMF_TRILINEAR_FILTER, true);
    meshComponent.node->setMaterialFlag(EMF_ANISOTROPIC_FILTER, false);


    /* --- DEBUG DRAWING --- */
    //meshComponent.node->setDebugDataVisible(EDS_NORMALS | EDS_BBOX_ALL);

    meshComponent.node->updateAbsolutePosition();

    meshComponent.selector = RenderManager::Get()->sceneManager()->createTriangleSelector(meshComponent.node);
    meshComponent.node->setTriangleSelector(meshComponent.selector);

    RenderManager::Get()->renderer()->getMaterialSwapper()->swapMaterials(meshComponent.node);

	if (entity.hasComponent<LightComponent>() && meshComponent.mesh == "_primitive_sphere")
	{
		auto lscale = entity.getComponent<LightComponent>().radius * DEBUG_MESH_LIGHT_SPHERE_SCALE;
		meshComponent.node->setScale(irr::core::vector3df(lscale, lscale, lscale));

		meshComponent.node->setVisible(false);

		meshComponent.node->setIsDebugObject(true);
	}
	else
	{
		meshComponent.node->setVisible(m_drawDebugSprites);
	}

    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        meshComponent.node->setIsDebugObject(true);
    }
}
void RenderSystem::setLightComponentData(anax::Entity& entity)
{
    auto& lightComponent = entity.getComponent<LightComponent>();
    auto &transform = entity.getComponent<TransformComponent>();

    if (lightComponent.node) {
        lightComponent.node->remove();
    }

    lightComponent.node =
        RenderManager::Get()->sceneManager()->addLightSceneNode(transform.node);

    // DEPRECATED
    //lightComponent.node->setPosition(transform.position + lightComponent.offset);

    lightComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        lightComponent.node->setIsDebugObject(true);
    }

	/*RenderManager::Get()->effect()->addShadowLight(SShadowLight(1024, transform.position, vector3df(0, 0, 0),
		lightComponent.color_diffuse, 1.0f, 10.0f, 89.99f * DEGTORAD));*/

    setLightData(lightComponent);
}
void RenderSystem::setBillboardComponentData(anax::Entity& entity)
{
    auto& spriteComponent = entity.getComponent<BillboardSpriteComponent>();
    auto &transform = entity.getComponent<TransformComponent>();

    if (spriteComponent.selectorNode) {
        spriteComponent.selectorNode->remove();
    }
    if (spriteComponent.selector) {
        spriteComponent.selector->drop();
    }
    if (spriteComponent.node) {
        spriteComponent.node->remove();
    }

    // Selector
    spriteComponent.selectorMesh =
        RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/sphere_low.obj");

    spriteComponent.selectorNode =
        RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(
            spriteComponent.selectorMesh,
            /*entity.getComponent<TransformComponent>().node*/nullptr,
            entity.getComponent<DescriptorComponent>().id);

    spriteComponent.selectorNode->setScale(vector3df(0.5f, 0.5f, 0.5f));
    spriteComponent.selector = RenderManager::Get()->sceneManager()->createTriangleSelector(spriteComponent.selectorNode);
    spriteComponent.selectorNode->setTriangleSelector(spriteComponent.selector);
    spriteComponent.selectorNode->setMaterialFlag(EMF_LIGHTING, false);
    spriteComponent.selectorNode->setMaterialFlag(EMF_WIREFRAME, true);
    spriteComponent.selectorNode->setMaterialFlag(EMF_FRONT_FACE_CULLING, true);
    spriteComponent.selectorNode->setVisible(m_drawDebugSprites);
    // DEPRECATED
    //spriteComponent.selectorNode->setPosition(transform.position);
    spriteComponent.selectorNode->setID(entity.getComponent<DescriptorComponent>().id);

    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        spriteComponent.selectorNode->setIsDebugObject(true);
    }

    spriteComponent.node =
        RenderManager::Get()->sceneManager()->addBillboardSceneNode(entity.getComponent<TransformComponent>().node);

    spriteComponent.node->setSize(dimension2d<f32>(spriteComponent.scale_x, spriteComponent.scale_y));

	///////////////////////////////////////////////////////////////////////////////////
	///// BUG: MASSIVE MEMORY LEAK, ALLOCATES ~30MB per billboard sprite, doesn't release it when entity is destroyed
	/////      Spawn a few to many and you get 1+ GB of memory leaked and a heap corruption crash.
	///////////////////////////////////////////////////////////////////////////////////
    if (spriteComponent.animated) {
        auto source_image = RenderManager::Get()->driver()->createImageFromFile(std::string(g_texture_path + spriteComponent.sprite).c_str());
        IImage* temp_image = RenderManager::Get()->driver()->createImage(source_image->getColorFormat(), irr::core::dimension2d<u32>(spriteComponent.split_x, spriteComponent.split_y));

        auto counter = 0;
        for (auto i = 0U; i < source_image->getDimension().Height / spriteComponent.split_y; i++) {
            for (auto j = 0U; j < source_image->getDimension().Width / spriteComponent.split_x; j++) {

                source_image->copyTo(temp_image, irr::core::vector2d<s32>(0, 0), irr::core::rect<int>(
                    spriteComponent.split_x * j, spriteComponent.split_y * i, spriteComponent.split_x * (i + 8), spriteComponent.split_y * (j + 8)));

                spriteComponent.texture_list.push_back(RenderManager::Get()->driver()->addTexture(std::to_string(counter++).c_str(), temp_image));
            }
        }

        temp_image->drop();
		source_image->drop();

        spriteComponent.node->setMaterialTexture(0, spriteComponent.texture_list.at(0));

        /* for (auto texture : spriteComponent.texture_list) {
             RenderManager::Get()->driver()->makeColorKeyTexture(texture, core::position2d<s32>(0, 0));
         }*/
    } /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else {
        spriteComponent.node->setMaterialTexture(0, RenderManager::Get()->driver()->getTexture(
            std::string(g_texture_path + spriteComponent.sprite).c_str()));
    }

    spriteComponent.node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF); // EMT_TRANSPARENT_ALPHA_CHANNEL_REF might be better for billboards sprites I guess? needs testing...
    spriteComponent.node->setMaterialFlag(EMF_LIGHTING, false);
    spriteComponent.node->setMaterialFlag(EMF_BILINEAR_FILTER, true);
    spriteComponent.node->setMaterialFlag(EMF_TRILINEAR_FILTER, true);
    spriteComponent.node->setMaterialFlag(EMF_ANISOTROPIC_FILTER, true);
    spriteComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

    spriteComponent.selectorNode->setPosition(spriteComponent.node->getAbsolutePosition());
}
void RenderSystem::setDebugSpriteComponentData(anax::Entity& entity)
{
    auto& debugspriteComponent = entity.getComponent<DebugSpriteComponent>();
    auto &transform = entity.getComponent<TransformComponent>();

    if (debugspriteComponent.selectorNode) {
        debugspriteComponent.selectorNode->remove();
    }
    if (debugspriteComponent.selector) {
        debugspriteComponent.selector->drop();
    }
    if (debugspriteComponent.node) {
        debugspriteComponent.node->remove();
    }

    // Selector
    debugspriteComponent.selectorMesh =
        RenderManager::Get()->sceneManager()->getMesh("content/mesh/primitive/sphere_low.obj");

    debugspriteComponent.selectorNode =
        RenderManager::Get()->sceneManager()->addAnimatedMeshSceneNode(
            debugspriteComponent.selectorMesh,
            /*entity.getComponent<TransformComponent>().node*/nullptr,
            entity.getComponent<DescriptorComponent>().id);

    debugspriteComponent.selectorNode->setScale(vector3df(0.5f, 0.5f, 0.5f));
    debugspriteComponent.selector = RenderManager::Get()->sceneManager()->createTriangleSelector(debugspriteComponent.selectorNode);
    debugspriteComponent.selectorNode->setTriangleSelector(debugspriteComponent.selector);
    debugspriteComponent.selectorNode->setMaterialFlag(EMF_LIGHTING, false);
    debugspriteComponent.selectorNode->setMaterialFlag(EMF_WIREFRAME, true);
    debugspriteComponent.selectorNode->setMaterialFlag(EMF_FRONT_FACE_CULLING, true);
    debugspriteComponent.selectorNode->setVisible(m_drawDebugSprites);
    // DEPRECATED
    //debugspriteComponent.selectorNode->setPosition(transform.position);
    debugspriteComponent.selectorNode->setID(entity.getComponent<DescriptorComponent>().id);

    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        debugspriteComponent.selectorNode->setIsDebugObject(true);
    }

    debugspriteComponent.node =
        RenderManager::Get()->sceneManager()->addBillboardSceneNode(entity.getComponent<TransformComponent>().node);

    debugspriteComponent.node->setSize(dimension2d<f32>(0.5f, 0.5f));
    // DEPRECATED
    //debugspriteComponent.node->setPosition(transform.position);
    if (entity.hasComponent<LightComponent>()) {
        auto color = entity.getComponent<LightComponent>().color_diffuse;
        debugspriteComponent.node->setColor(SColor(255, static_cast<irr::u32>(color.r * 255.0), static_cast<irr::u32>(color.g * 255.0f), static_cast<irr::u32>(color.b * 255.0f)));
    }

    debugspriteComponent.node->setMaterialTexture(0, RenderManager::Get()->driver()->getTexture(
        std::string(g_texture_path + debugspriteComponent.sprite).c_str()));
    debugspriteComponent.node->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
    debugspriteComponent.node->setMaterialFlag(EMF_LIGHTING, false);
    debugspriteComponent.node->setMaterialFlag(EMF_BILINEAR_FILTER, true);
    debugspriteComponent.node->setMaterialFlag(EMF_TRILINEAR_FILTER, true);
    debugspriteComponent.node->setMaterialFlag(EMF_ANISOTROPIC_FILTER, true);
    debugspriteComponent.node->setVisible(m_drawDebugSprites);
    debugspriteComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

    if (entity.getComponent<DescriptorComponent>().isDebug && Engine::Get()->isGameMode()) {
        debugspriteComponent.node->setIsDebugObject(true);
    }

    debugspriteComponent.selectorNode->setPosition(debugspriteComponent.node->getAbsolutePosition());
}

void RenderSystem::onEntityAdded(Entity& entity)
{
	if (entity.hasComponent<MeshComponent>()) {
        setMeshComponentData(entity);
	}

    if (entity.hasComponent<DebugMeshComponent>()) {
        setDebugMeshComponentData(entity);
    }

	if (entity.hasComponent<LightComponent>()) {
        setLightComponentData(entity);
	}

    if (entity.hasComponent<BillboardSpriteComponent>()) {
        setBillboardComponentData(entity);
    }

	if (entity.hasComponent<DebugSpriteComponent>()) {
        setDebugSpriteComponentData(entity);
	}

    forceTransformUpdate();
}

void RenderSystem::onEntityRemoved(Entity& entity) {
	if (entity.hasComponent<MeshComponent>()) {
		//RenderManager::Get()->driver()->removeOcclusionQuery(entity.getComponent<MeshComponent>().node);
		entity.getComponent<MeshComponent>().node->remove();
        entity.getComponent<MeshComponent>().selector->drop();
	}
	
	if (entity.hasComponent<LightComponent>()) {
        entity.getComponent<LightComponent>().node->remove();
	}

	if (entity.hasComponent<BillboardSpriteComponent>()) {
		entity.getComponent<BillboardSpriteComponent>().node->remove();
		entity.getComponent<BillboardSpriteComponent>().selectorNode->remove();
        entity.getComponent<BillboardSpriteComponent>().selector->drop();
	}

    if (entity.hasComponent<DebugSpriteComponent>()) {
        entity.getComponent<DebugSpriteComponent>().node->remove();
        entity.getComponent<DebugSpriteComponent>().selectorNode->remove();
        entity.getComponent<DebugSpriteComponent>().selector->drop();
    }
}

void RenderSystem::update()
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
		auto& render = entity.getComponent<RenderComponent>();
		auto& transform = entity.getComponent<TransformComponent>();
        
		if (entity.hasComponent<MeshComponent>()) {
			auto& node = entity.getComponent<MeshComponent>().node;

            node->setID(entity.getComponent<DescriptorComponent>().id);

			node->setVisible(render.isVisible);

			node->updateAbsolutePosition();
		}
        
		if (entity.hasComponent<LightComponent>()) {
			auto& lightComponent = entity.getComponent<LightComponent>();

            lightComponent.node->setID(entity.getComponent<DescriptorComponent>().id);
		    
			lightComponent.node->setVisible(render.isVisible && lightComponent.visible);
		    
			if (lightComponent.update_component_data) {
				lightComponent.update_component_data = false;
				setLightData(lightComponent);

				if (entity.hasComponent<DebugSpriteComponent>()) {
					auto color = lightComponent.color_diffuse;
					entity.getComponent<DebugSpriteComponent>().node->setColor(SColor(255, static_cast<irr::u32>(color.r * 255.0f), static_cast<irr::u32>(color.g * 255.0f), static_cast<irr::u32>(color.b * 255.0f)));
				}
			}
		}

        if (entity.hasComponent<BillboardSpriteComponent>()) {
            entity.getComponent<BillboardSpriteComponent>().node->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<BillboardSpriteComponent>().selectorNode->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<BillboardSpriteComponent>().selectorNode->setPosition(entity.getComponent<BillboardSpriteComponent>().node->getAbsolutePosition());

            if (Engine::Get()->isGameMode()) {
                if (entity.getComponent<BillboardSpriteComponent>().animated) {
                    auto &billboard = entity.getComponent<BillboardSpriteComponent>();

                    if (billboard.counter < billboard.texture_list.size() - 1 && !billboard.finished) {
                        if (Engine::Get()->getCurrentTime() - billboard.last_update_time > (1000.0f / billboard.fps)) {
                            billboard.counter++;
                            billboard.last_update_time = Engine::Get()->getCurrentTime();
                        }
                    } else {
                        billboard.counter = 0;

                        if (!billboard.loop) {
                            billboard.finished = true;

							if (billboard.destroyOnFinish)
							{
								WorldManager::Get()->killEntityByID(entity.getComponent<DescriptorComponent>().id);
							}
                        }
                    }

                    billboard.node->setMaterialTexture(0, billboard.texture_list.at(billboard.counter));
                }
            }
        }

		if (entity.hasComponent<DebugSpriteComponent>()) {
			entity.getComponent<DebugSpriteComponent>().node->setID(entity.getComponent<DescriptorComponent>().id);
			entity.getComponent<DebugSpriteComponent>().selectorNode->setID(entity.getComponent<DescriptorComponent>().id);
			entity.getComponent<DebugSpriteComponent>().selectorNode->setPosition(entity.getComponent<DebugSpriteComponent>().node->getAbsolutePosition());
		}

        if (entity.hasComponent<DebugMeshComponent>()) {
            auto& node = entity.getComponent<DebugMeshComponent>().node;

            node->setID(entity.getComponent<DescriptorComponent>().id);

			if (g_sceneInteractor.getConfiguration().drawPointLightBounds)
			{
				if (entity.hasComponent<LightComponent>())
				{
					auto lscale = entity.getComponent<LightComponent>().radius * DEBUG_MESH_LIGHT_SPHERE_SCALE;

					node->setScale(irr::core::vector3df(lscale, lscale, lscale));

					node->setVisible(false);
				}
			}
        }
    }

	if (m_applyDebugSpriteVisSetting) {
		for (auto& entity : entities) {
			if (entity.hasComponent<DebugSpriteComponent>()) {
				entity.getComponent<DebugSpriteComponent>().node->setVisible(m_drawDebugSprites);
				entity.getComponent<DebugSpriteComponent>().selectorNode->setVisible(m_drawDebugSprites);
			}
            if (entity.hasComponent<DebugMeshComponent>()) {
				if (!entity.hasComponent<LightComponent>())
				{
					entity.getComponent<DebugMeshComponent>().node->setVisible(m_drawDebugSprites);
				}
				else
				{
					entity.getComponent<DebugMeshComponent>().node->setVisible(false);
				}
            }
		}
		m_applyDebugSpriteVisSetting = false;
	}
}

void RenderSystem::forceTransformUpdate(bool updateLights)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto& render = entity.getComponent<RenderComponent>();
        auto& transform = entity.getComponent<TransformComponent>();

        if (entity.hasComponent<MeshComponent>()) {
            auto& node = entity.getComponent<MeshComponent>().node;

            node->setID(entity.getComponent<DescriptorComponent>().id);

            node->setVisible(render.isVisible);
        }

        if (entity.hasComponent<LightComponent>()) {
            auto& lightComponent = entity.getComponent<LightComponent>();

            lightComponent.node->setID(entity.getComponent<DescriptorComponent>().id);

            lightComponent.node->setVisible(render.isVisible && lightComponent.visible);

            if (lightComponent.update_component_data) {
                lightComponent.update_component_data = false;
                setLightData(lightComponent);

                if (entity.hasComponent<DebugSpriteComponent>()) {
                    auto color = lightComponent.color_diffuse;
                    entity.getComponent<DebugSpriteComponent>().node->setColor(SColor(255, static_cast<irr::u32>(color.r * 255.0f), static_cast<irr::u32>(color.g * 255.0f), static_cast<irr::u32>(color.b * 255.0f)));
                }
            }
        }

        if (entity.hasComponent<BillboardSpriteComponent>()) {
            entity.getComponent<BillboardSpriteComponent>().node->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<BillboardSpriteComponent>().selectorNode->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<BillboardSpriteComponent>().selectorNode->setPosition(entity.getComponent<BillboardSpriteComponent>().node->getAbsolutePosition());
        }

        if (entity.hasComponent<DebugSpriteComponent>()) {
            entity.getComponent<DebugSpriteComponent>().node->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<DebugSpriteComponent>().selectorNode->setID(entity.getComponent<DescriptorComponent>().id);
            entity.getComponent<DebugSpriteComponent>().selectorNode->setPosition(entity.getComponent<DebugSpriteComponent>().node->getAbsolutePosition());
        }

        if (entity.hasComponent<DebugMeshComponent>()) {
            auto& node = entity.getComponent<DebugMeshComponent>().node;

            node->setID(entity.getComponent<DescriptorComponent>().id);

			if (g_sceneInteractor.getConfiguration().drawPointLightBounds)
			{
				if (entity.hasComponent<LightComponent>())
				{
					auto lscale = entity.getComponent<LightComponent>().radius * DEBUG_MESH_LIGHT_SPHERE_SCALE;

					node->setScale(irr::core::vector3df(lscale, lscale, lscale));

					node->setVisible(false);
				}
			}
        }
    }

	if (m_applyDebugSpriteVisSetting) {
		for (auto& entity : entities) {
			if (entity.hasComponent<DebugSpriteComponent>()) {
				entity.getComponent<DebugSpriteComponent>().node->setVisible(m_drawDebugSprites);
				entity.getComponent<DebugSpriteComponent>().selectorNode->setVisible(m_drawDebugSprites);
			}
			if (entity.hasComponent<DebugMeshComponent>()) {
				if (!entity.hasComponent<LightComponent>())
				{
					entity.getComponent<DebugMeshComponent>().node->setVisible(m_drawDebugSprites);
				}
				else
				{
					entity.getComponent<DebugMeshComponent>().node->setVisible(false);
				}
			}
		}
		m_applyDebugSpriteVisSetting = false;
	}
}

void RenderSystem::remove(std::string name)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {

        if (entity.getComponent<DescriptorComponent>().name == name) {

            entity.getComponent<MeshComponent>().node->remove();
            return;
        }
    }
}

void RenderSystem::setVisible(std::string name, bool visible)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {

        if (entity.getComponent<DescriptorComponent>().name == name) {
			if (entity.hasComponent<MeshComponent>()) {
				entity.getComponent<MeshComponent>().node->setVisible(visible);
			}
            return;
        }
    }
}

void RenderSystem::setFrame(entityid id, irr::s32 frame)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(id);

	if (entity.hasComponent<MeshComponent>()) {

		auto& render = entity.getComponent<MeshComponent>();

		if (render.isAnimated) {
		    render.node->setCurrentFrame(static_cast<irr::f32>(frame));
		}

	    if (render.isAnimated && render.trimesh->getFrameCount() < 2) {
			spdlog::warn("RenderSystem::setFrame failed to play animation, no animation frames exist in node");
	    }

	}
}

void RenderSystem::setFrameLoop(entityid id, irr::s32 begin, irr::s32 end)
{
	auto& entity = WorldManager::Get()->managerSystem()->getEntityByID(id);

	if (entity.hasComponent<MeshComponent>()) {

		auto& render = entity.getComponent<MeshComponent>();

		if (render.isAnimated) { render.node->setFrameLoop(begin, end); }

	}
}

void RenderSystem::setFrame(std::string name, irr::s32 frame)
{
    auto& entity = WorldManager::Get()->managerSystem()->getEntityByName(name);

    if (entity.hasComponent<MeshComponent>()) {

        auto& render = entity.getComponent<MeshComponent>();
		
		if (render.isAnimated) { render.node->setCurrentFrame(static_cast<irr::f32>(frame)); }
    }
}

void RenderSystem::setFrameLoop(std::string name, irr::s32 begin, irr::s32 end)
{
    auto& entity = WorldManager::Get()->managerSystem()->getEntityByName(name);

    if (entity.hasComponent<MeshComponent>()) {

        auto& render = entity.getComponent<MeshComponent>();

        if (render.isAnimated) { render.node->setFrameLoop(begin, end); }

    }
}

void RenderSystem::playAnimation(entityid id, std::string animation)
{
	auto& entities = getEntities();

	for (auto& entity : entities) {
		if (entity.hasComponent<MeshComponent>()) {
			if (entity.getComponent<DescriptorComponent>().id == id) { // Don't 'for loop' for an ID its stupid and slow

				auto start = 0U, end = 0U;
				auto& render = entity.getComponent<MeshComponent>();

				for (sAnimationData s : render.animationList) {
					if (s.name == animation) {
						start = s.frames.X;
						end = s.frames.Y;

                        render.lastPlayedAnimation = sAnimationData(s.name, s.frames.X, s.frames.Y, s.loop);
					}
				}

                render.node->setLoopMode(false);
				render.node->setFrameLoop(start, end);

				return;
			}
		}
	}
}

void RenderSystem::loopAnimation(entityid id, std::string animation)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        if (entity.hasComponent<MeshComponent>()) {
            if (entity.getComponent<DescriptorComponent>().id == id) { // Don't 'for loop' for an ID its stupid and slow

                auto start = 0U, end = 0U;
                auto& render = entity.getComponent<MeshComponent>();

                for (sAnimationData s : render.animationList) {
                    if (s.name == animation) {
                        start = s.frames.X;
                        end = s.frames.Y;
                    }
                }

                render.node->setLoopMode(true);
                render.node->setFrameLoop(start, end);

                return;
            }
        }
    }
}

void RenderSystem::playAnimation(const anax::Entity& entity, std::string animation)
{
	if (entity.hasComponent<MeshComponent>()) {
		auto start = 0U, end = 0U;
		auto& render = entity.getComponent<MeshComponent>();

		for (sAnimationData s : render.animationList) {
			if (s.name == animation) {
				start = s.frames.X;
				end = s.frames.Y;
			}
		}

		render.node->setFrameLoop(start, end);
	}
}

bool RenderSystem::getEntityAABBIntersection(entityid e1, entityid e2)
{
	auto& entity1 = WorldManager::Get()->managerSystem()->getEntityByID(e1);
	auto& entity2 = WorldManager::Get()->managerSystem()->getEntityByID(e2);

	if (entity1.isValid() && entity2.isValid()) {
		if (entity1.hasComponent<MeshComponent>() && entity2.hasComponent<MeshComponent>()) {
			return RenderManager::getAABBIntersection(entity1.getComponent<MeshComponent>().node, entity2.getComponent<MeshComponent>().node);
		}
	}

	spdlog::warn("Entity invalid/missing required component in RenderSystem::getEntityAABBIntersection");

	return false;
}

bool RenderSystem::swapMesh(entityid id, std::string file)
{
	auto& entities = getEntities();

	for (auto& entity : entities) {
		if (entity.hasComponent<MeshComponent>()) {
			if (entity.getComponent<DescriptorComponent>().id == id) {
				if (entity.getComponent<MeshComponent>().trimesh && entity.getComponent<MeshComponent>().node) {
					auto mesh = RenderManager::Get()->sceneManager()->getMesh(file.c_str());
				    
					if (mesh) {
						entity.getComponent<MeshComponent>().trimesh = mesh;
						entity.getComponent<MeshComponent>().node->setMesh(entity.getComponent<MeshComponent>().trimesh);
						return true;
					}
				}
            }
		}
	}

	spdlog::warn("No ID match in RenderSystem::swapMesh");

	return false;
}

// CRITICAL BUG: If 'texture_id' is greater than '... textures().size()' fatal program termination will occur
void RenderSystem::swapTexture(entityid id, unsigned int texture_id)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        if (entity.hasComponent<MeshComponent>()) {
            if (entity.getComponent<DescriptorComponent>().id == id) {
                if (entity.getComponent<MeshComponent>().node) {
                    if (texture_id > entity.getComponent<MeshComponent>().node->getMaterialCount()) {
                        return;
                    }

                    entity.getComponent<MeshComponent>().node->setMaterialTexture(0, 
						RenderManager::Get()->driver()->getTexture(
							entity.getComponent<MeshComponent>().textures.at(texture_id).c_str()));
                    return;
                }
            }
        }
    }

    spdlog::warn("No ID match in RenderSystem::swapTexture");
}

void RenderSystem::setNodeMaterialType(entityid id, irr::video::E_MATERIAL_TYPE material_type)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        if (entity.hasComponent<MeshComponent>()) {
            if (entity.getComponent<DescriptorComponent>().id == id) {
                if (entity.getComponent<MeshComponent>().node) {
                    entity.getComponent<MeshComponent>().node->setMaterialType(material_type);
                    //RenderManager::Get()->renderer()->getMaterialSwapper()->swapMaterials(entity.getComponent<MeshComponent>().node);
                    return;
                }
            }
        }
    }

    spdlog::warn("No ID match in RenderSystem::swapTexture");
}
