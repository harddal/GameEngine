#include "PhysicsSystem.h"

#include "Engine/Engine.h"

using namespace anax;
using namespace irr;
using namespace core;
using namespace physx;

// TODO: Flushs simulation memory, implement a fixed call every ~1000 frames or so?
// gScene->flushSimulation(false);

// TODO: Implement more than just box, sphere, and static mesh colliders
void PhysicsSystem::onEntityAdded(Entity& entity)
{
    vector3df
          position = entity.getComponent<TransformComponent>().getPosition(),
          rotation = entity.getComponent<TransformComponent>().getRotation(),
          scale    = entity.getComponent<TransformComponent>().getScale();

    switch (entity.getComponent<PhysicsComponent>().type) {
        case PCT_PLANE: {
			spdlog::error(
				"Entity \'" + entity.getComponent<DescriptorComponent>().name + "\' contains unimplemented feature: PCT_PLANE");
        }
            break;
        case PCT_BOX:
        {
            auto& actor = entity.getComponent<PhysicsComponent>().actor;

            entity.getComponent<PhysicsComponent>().transform = new PxTransform(
                PxVec3(
                    position.X * IRR_PHYSX_POS_SCALAR,
                    position.Y * IRR_PHYSX_POS_SCALAR,
                    position.Z * IRR_PHYSX_POS_SCALAR),
                Math::EulerToQuaternion(rotation));

            // Use the meshes AABB half-extent to correctly size the collider
            if (entity.hasComponent<MeshComponent>()) {
                if (entity.getComponent<MeshComponent>().isPrimitive) {
                    entity.getComponent<PhysicsComponent>().dimensions = new
                        PxVec3(
                            scale.X * IRR_PHYSX_DIM_SCALAR,
                            scale.Y * IRR_PHYSX_DIM_SCALAR,
                            scale.Z * IRR_PHYSX_DIM_SCALAR);
                }
                else {
                    auto extent = entity.getComponent<MeshComponent>().node->getTransformedBoundingBox().getExtent();
                    entity.getComponent<PhysicsComponent>().dimensions = new
                        PxVec3(
                            extent.X * IRR_PHYSX_DIM_SCALAR,
                            extent.Y * IRR_PHYSX_DIM_SCALAR,
                            extent.Z * IRR_PHYSX_DIM_SCALAR);
                }
            }
            else {
                entity.getComponent<PhysicsComponent>().dimensions = new
                    PxVec3(
                        scale.X * IRR_PHYSX_DIM_SCALAR,
                        scale.Y * IRR_PHYSX_DIM_SCALAR,
                        scale.Z * IRR_PHYSX_DIM_SCALAR);
           }

            PxBoxGeometry geometry(*entity.getComponent<PhysicsComponent>().dimensions);
            actor = PxCreateDynamic(*PhysicsManager::Get()->physics(), *entity.getComponent<PhysicsComponent>().transform,
                                    geometry, *PhysicsManager::Get()->getDefaultMaterial(), entity.getComponent<PhysicsComponent>().density);

            actor->setName(entity.getComponent<DescriptorComponent>().name.c_str());
			actor->userData = reinterpret_cast<void*>(entity.getComponent<DescriptorComponent>().id);

            actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, entity.getComponent<PhysicsComponent>().kinematic);

			// DEBUG: Set the ID to the actors user data
			actor->userData = reinterpret_cast<void*>(entity.getComponent<DescriptorComponent>().id);

			PhysicsManager::Get()->scene()->addActor(*actor);
        }
            break;
        case PCT_SPHERE:
        {
            auto& actor = entity.getComponent<PhysicsComponent>().actor;

            entity.getComponent<PhysicsComponent>().transform = new PxTransform(
                PxVec3(
                    position.X * IRR_PHYSX_POS_SCALAR,
                    position.Y * IRR_PHYSX_POS_SCALAR,
                    position.Z * IRR_PHYSX_POS_SCALAR),
				Math::EulerToQuaternion(rotation));

            // Use the meshes AABB half-extent to correctly size the collider
            if (entity.hasComponent<RenderComponent>()) {
                if (entity.getComponent<MeshComponent>().isPrimitive) {
                    entity.getComponent<PhysicsComponent>().dimensions = new
                        PxVec3(
                            scale.X * IRR_PHYSX_DIM_SCALAR,
                            scale.Y * IRR_PHYSX_DIM_SCALAR,
                            scale.Z * IRR_PHYSX_DIM_SCALAR);
                }
                else {
                    auto extent = entity.getComponent<MeshComponent>().node->getTransformedBoundingBox().getExtent();
                    entity.getComponent<PhysicsComponent>().dimensions = new
                        PxVec3(
                            extent.X * IRR_PHYSX_DIM_SCALAR,
                            extent.Y * IRR_PHYSX_DIM_SCALAR,
                            extent.Z * IRR_PHYSX_DIM_SCALAR);
                }
            }
            else {
                entity.getComponent<PhysicsComponent>().dimensions = new
                    PxVec3(
                        scale.X * IRR_PHYSX_DIM_SCALAR,
                        scale.Y * IRR_PHYSX_DIM_SCALAR,
                        scale.Z * IRR_PHYSX_DIM_SCALAR);
            }

            // Only get y dimension, should be fine if it's a sphere
            PxSphereGeometry geometry(entity.getComponent<PhysicsComponent>().dimensions->y);
            actor = PxCreateDynamic(*PhysicsManager::Get()->physics(), *entity.getComponent<PhysicsComponent>().transform,
                                    geometry, *PhysicsManager::Get()->getDefaultMaterial(), entity.getComponent<PhysicsComponent>().density);

            actor->setName(entity.getComponent<DescriptorComponent>().name.c_str());
			actor->userData = reinterpret_cast<void*>(entity.getComponent<DescriptorComponent>().id);

            actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, entity.getComponent<PhysicsComponent>().kinematic);

			// DEBUG: Set the ID to the actors user data
			actor->userData = reinterpret_cast<void*>(entity.getComponent<DescriptorComponent>().id);

			PhysicsManager::Get()->scene()->addActor(*actor);
        }
            break;
        case PCT_CAPSULE:
        {
			spdlog::error(
				"Entity \'" + entity.getComponent<DescriptorComponent>().name + "\' contains unimplemented feature: PCT_CAPSULE");
        }
            break;
        case PCT_CONVEX: // BUG: Currently broken
        {
            spdlog::error(
				"Entity \'" + entity.getComponent<DescriptorComponent>().name + "\' contains unimplemented feature: PCT_CONVEX");
			//auto& actor = entity.getComponent<PhysicsComponent>().actor;
 
   //         if (entity.hasComponent<MeshComponent>()) {
   //             
   //         }
			//else {
			//	log::write("Failed to cook convex mesh, entity has missing/invalid MeshComponent", LOG_WARNING_CRITICAL);
			//	break;
			//}

			//Assimp::Importer importer;
			//auto scene = const_cast<aiScene*>(importer.ReadFile("content/mesh/primitive/cube.obj"/*entity.getComponent<MeshComponent>().mesh*/, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
			//	aiProcess_OptimizeMeshes | aiProcess_ConvertToLeftHanded));

			//if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			//	log::write("Failed to import physics mesh, error: " + std::string(importer.GetErrorString()), LOG_WARNING_CRITICAL);
			//	return;
			//}

   //         std::vector<aiMesh*> meshes;
			//for (auto i = 0U; i < scene->mRootNode->mNumMeshes; i++) {
			//	meshes.push_back(scene->mMeshes[scene->mRootNode->mMeshes[i]]);
			//}

			//std::vector<PxVec3> vertices;
			//for (aiMesh* m : meshes) {
			//	for (auto i = 0U; i < m->mNumVertices; i++) {
			//		PxVec3 v;

			//		v.x = m->mVertices[i].x;
			//		v.y = m->mVertices[i].y;
			//		v.z = m->mVertices[i].z;
			//		log::write("Vx: " + std::to_string(v.x) + "Vy: " + std::to_string(v.y) + "Vz: " + std::to_string(v.z), LOG_DEBUG);
			//		vertices.push_back(v);
			//	}
			//}
   //         
			//importer.FreeScene();

			//void* verts = &vertices[0];
   //         
			//PxConvexMeshDesc convexDesc;
			//convexDesc.points.count = vertices.size();
			//convexDesc.points.stride = sizeof(PxVec3);
			//convexDesc.points.data = verts;
			//convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

			//PxDefaultMemoryOutputStream buf;
			//PxConvexMeshCookingResult::Enum result;
			//if (!PhysicsManager::Get()->cooking()->cookConvexMesh(convexDesc, buf, &result)) {
			//	log::write("Failed to cook convex mesh, cooking returned 0", LOG_WARNING_CRITICAL);
			//	break;
			//}
			//	
			//PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			//PxConvexMesh* convexMesh = PhysicsManager::Get()->physics()->createConvexMesh(input);
   //         
			////PxShape* aConvexShape = PxRigidActor::createShape(PxConvexMeshGeometry(convexMesh), *PhysicsManager::Get()->getDefaultMaterial());

   //         actor = PxCreateDynamic(*PhysicsManager::Get()->physics(), *entity.getComponent<PhysicsComponent>().transform,
			//	PxConvexMeshGeometry(convexMesh), *PhysicsManager::Get()->getDefaultMaterial(), entity.getComponent<PhysicsComponent>().density);
			//actor->setName(entity.getComponent<DescriptorComponent>().name.c_str());
			//PhysicsManager::Get()->m_scene->addActor(*actor);
        }
            break;
        case PCT_TRIANGLE: // TODO: Implement deletion of trimeshes
        {
			if (entity.getComponent<PhysicsComponent>().collisionMesh.length() > 0)
			{
				PhysicsManager::Get()->cookTriangleMeshFromFile(
					entity.getComponent<PhysicsComponent>().collisionMesh,
					entity.getComponent<PhysicsComponent>().actor,
					PxVec3(
						position.X * IRR_PHYSX_POS_SCALAR,
						position.Y * IRR_PHYSX_POS_SCALAR,
						position.Z * IRR_PHYSX_POS_SCALAR),
					Math::EulerToQuaternion(rotation),
					PxVec3(
						scale.X * IRR_PHYSX_POS_SCALAR,
						scale.Y * IRR_PHYSX_POS_SCALAR,
						scale.Z * IRR_PHYSX_POS_SCALAR));
			}
			else
			{
				PhysicsManager::Get()->cookTriangleMeshFromMemory(
					entity.getComponent<MeshComponent>().trimesh,
					entity.getComponent<PhysicsComponent>().actor,
					PxVec3(
						position.X * IRR_PHYSX_POS_SCALAR,
						position.Y * IRR_PHYSX_POS_SCALAR,
						position.Z * IRR_PHYSX_POS_SCALAR),
					Math::EulerToQuaternion(rotation),
					PxVec3(
						scale.X * IRR_PHYSX_POS_SCALAR,
						scale.Y * IRR_PHYSX_POS_SCALAR,
						scale.Z * IRR_PHYSX_POS_SCALAR));
			}

			entity.getComponent<PhysicsComponent>().kinematic = true;
        }
            break;
        default:
            spdlog::error(
				"Invalid PHYSICS_COLLIDER_TYPE in entity \'" + entity.getComponent<DescriptorComponent>().name + "\' PhysicsSystem::onEntityAdded()");
            return;
    }
}

void PhysicsSystem::onEntityRemoved(anax::Entity& entity)
{
    if (entity.getComponent<PhysicsComponent>().type != PCT_TRIANGLE) {
        PhysicsManager::Get()->scene()->removeActor(*entity.getComponent<PhysicsComponent>().actor);
    }
}

void PhysicsSystem::init()
{
	m_defaultMaterial = PhysicsManager::Get()->getDefaultMaterial();
	
    WorldManager::Get()->world()->refresh();
}

void PhysicsSystem::update(float dt)
{
    auto& entities = getEntities();
    for (auto& entity : entities) 
	{
        if (entity.getComponent<PhysicsComponent>().type != PHYSICS_COLLIDER_TYPE::PCT_TRIANGLE) 
		{
			auto& physics = entity.getComponent<PhysicsComponent>();

        	auto& actor = physics.actor;

            if (actor->isSleeping()) { continue; }

            PxU32 nShapes = actor->getNbShapes();
            PxShape** shapes = new PxShape *[nShapes];
            actor->getShapes(shapes, nShapes);

            PxFilterData filterData;

            while (nShapes--) 
			{
                // NOTE: Move this possibly for optimization
                filterData.word0 = RAYCAST_HIT_GROUP::RHG_DYNAMIC;
                shapes[nShapes]->setQueryFilterData(filterData);

                PxGeometryType::Enum type = shapes[nShapes]->getGeometryType();

                switch (type) {
                    case PxGeometryType::ePLANE:
                    {
                        //PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                        //                                           *actor);

                        ////PxPlaneGeometry bg; // Is this neccesary?
                        ////shapes[nShapes]->getPlaneGeometry(bg); <-

                        //auto& transform = entity.getComponent<TransformComponent>();
                        //transform.position = vector3df(pT.p.x, pT.p.y, pT.p.z);
                        //transform.rotation = math::quaternionToEuler(pT.q);
                    }
                        break;
                    case PxGeometryType::eBOX:
                    {
                        PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *actor);

                        //PxBoxGeometry bg; // Is this neccesary?
                        //shapes[nShapes]->getBoxGeometry(bg); <-

                        auto& transform = entity.getComponent<TransformComponent>();
                        transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                        transform.setRotation(Math::QuaternionToEuler(pT.q));
                    }
                        break;
                    case PxGeometryType::eSPHERE:
                    {
                        PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                                                                   *actor);

                        //PxSphereGeometry bg; // Is this neccesary?
                        //shapes[nShapes]->getSphereGeometry(bg); <-

                        auto& transform = entity.getComponent<TransformComponent>();
                        transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                        transform.setRotation(Math::QuaternionToEuler(pT.q));
                    }
                        break;
                    case PxGeometryType::eCAPSULE:
                    {
                        //PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                        //                                           *actor);

                        ////PxCapsuleGeometry bg; // Is this neccesary?
                        ////shapes[nShapes]->getCapsuleGeometry(bg); <-

                        //auto& transform = entity.getComponent<TransformComponent>();
                       // transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                        //transform.setRotation(math::quaternionToEuler(pT.q));
                    }
                        break;
                    case PxGeometryType::eCONVEXMESH: 
                    {
						//PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
						//	*actor);

						////PxBoxGeometry bg; // Is this neccesary?
						////shapes[nShapes]->getBoxGeometry(bg); <-

						//auto& transform = entity.getComponent<TransformComponent>();
                        //transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                        //transform.setRotation(math::quaternionToEuler(pT.q));
                    }
                        break;
                    default:
                        break;
                }
            }

            delete[] shapes;
            shapes = nullptr;

			auto test_point = entity.getComponent<TransformComponent>().position;

			for (auto& zone : WorldManager::Get()->gameplaySystem()->getWaterZones())
			{
				if (test_point.X <= zone.second.X && test_point.X >= zone.first.X &&
					test_point.Y <= zone.second.Y && test_point.Y >= zone.first.Y &&
					test_point.Z <= zone.second.Z && test_point.Z >= zone.first.Z)
				{
					if (physics.actor && !physics.kinematic)
					{
						physics.actor->setLinearDamping(.5);
						physics.actor->addForce(PxVec3(0.0, 20, 0.0));
					}
				}
			}
        }
    }

    //for (auto& l : m_raycastListeners)
    //{
    //	if (m_raycastData.block.actor && hasRaycasted)
    //	{
    //		if ()
    //		{
    //			l->onRaycastHit(game, &m_raycastData);
    //			hasRaycasted = false;
    //		}
    //	}
    //}
}

void PhysicsSystem::cleanup()
{
    
}

void PhysicsSystem::applyForce(std::string name, PxVec3 force)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto actor = entity.getComponent<PhysicsComponent>().actor;
		if (actor) {
			if (std::string(actor->getName()) == name) { actor->addForce(force); }
		}
    }
}

vector3df PhysicsSystem::getKinematicActorPosition(std::string name)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto actor = entity.getComponent<PhysicsComponent>().actor;
        if (actor) {
            if (std::string(actor->getName()) == name) {
                PxTransform t;
                if (actor->getKinematicTarget(t)) {
                    return vector3df(t.p.x, t.p.y, t.p.z);
                }
            }
        }
    }

    spdlog::error("PhysX actor \'" + name + "\' is not kinematic in \'PhysicsSystem::getKinematicActorPosition\'");
    return vector3df();
}
void PhysicsSystem::setKinematicActorPosition(std::string name, vector3df position)
{
    auto& entities = getEntities();

    for (auto& entity : entities) {
        auto actor = entity.getComponent<PhysicsComponent>().actor;
        if (actor) {
            if (std::string(actor->getName()) == name) {
                auto& tr = entity.getComponent<TransformComponent>();

                actor->setKinematicTarget(PxTransform(PxVec3(position.X, position.Y, position.Z), Math::EulerToQuaternion(tr.getRotation())));

                // DEBUG: Same code as PhysicsSystem::Update(), forces transform update on kinematic actor position change
                // DEBUG: Set the ID to the actors userdata
                actor->userData = reinterpret_cast<void*>(entity.getComponent<DescriptorComponent>().id);

                PxU32 nShapes = entity.getComponent<PhysicsComponent>().actor->getNbShapes();
                PxShape** shapes = new PxShape *[nShapes];
                entity.getComponent<PhysicsComponent>().actor->getShapes(shapes, nShapes);

                PxFilterData filterData;

                while (nShapes--) {
                    // NOTE: Move this possibly for optimization
                    filterData.word0 = RAYCAST_HIT_GROUP::RHG_DYNAMIC;
                    shapes[nShapes]->setQueryFilterData(filterData);

                    PxGeometryType::Enum type = shapes[nShapes]->getGeometryType();

                    switch (type) {
                    case PxGeometryType::ePLANE:
                        {
                            //PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                            //                                           *actor);

                            ////PxPlaneGeometry bg; // Is this neccesary?
                            ////shapes[nShapes]->getPlaneGeometry(bg); <-

                            //auto& transform = entity.getComponent<TransformComponent>();
                            //transform.position = vector3df(pT.p.x, pT.p.y, pT.p.z);
                            //transform.rotation = math::quaternionToEuler(pT.q);
                        }
                        break;
                    case PxGeometryType::eBOX:
                        {
                            PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                                                                       *actor);

                            //PxBoxGeometry bg; // Is this neccesary?
                            //shapes[nShapes]->getBoxGeometry(bg); <-

                            auto& transform = entity.getComponent<TransformComponent>();
                            transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                            transform.setRotation(Math::QuaternionToEuler(pT.q));
                        }
                        break;
                    case PxGeometryType::eSPHERE:
                        {
                            PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                                                                       *actor);

                            //PxSphereGeometry bg; // Is this neccesary?
                            //shapes[nShapes]->getSphereGeometry(bg); <-

                            auto& transform = entity.getComponent<TransformComponent>();
                            transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                            transform.setRotation(Math::QuaternionToEuler(pT.q));
                        }
                        break;
                    case PxGeometryType::eCAPSULE:
                        {
                            //PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                            //                                           *actor);

                            ////PxCapsuleGeometry bg; // Is this neccesary?
                            ////shapes[nShapes]->getCapsuleGeometry(bg); <-

                            //auto& transform = entity.getComponent<TransformComponent>();
                            // transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                            //transform.setRotation(math::quaternionToEuler(pT.q));
                        }
                        break;
                    case PxGeometryType::eCONVEXMESH:
                        {
                            //PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes],
                            //	*actor);

                            ////PxBoxGeometry bg; // Is this neccesary?
                            ////shapes[nShapes]->getBoxGeometry(bg); <-

                            //auto& transform = entity.getComponent<TransformComponent>();
                            //transform.setPosition(vector3df(pT.p.x, pT.p.y, pT.p.z));
                            //transform.setRotation(math::quaternionToEuler(pT.q));
                        }
                        break;
                    default:
                        break;
                    }
                }

                delete[] shapes;
                shapes = nullptr;
                
                return;
            }
        }
    }

    spdlog::error("PhysX actor \'" + name + "\' is not kinematic in \'PhysicsSystem::getKinematicActorPosition\'");
}

void CCTSubsystem::onEntityAdded(anax::Entity& entity)
{
    entity.getComponent<CCTComponent>().controller = PhysicsManager::Get()->createCCT(m_capsuleDesc);

    auto pos = entity.getComponent<TransformComponent>().getPosition();
    entity.getComponent<CCTComponent>().controller->setPosition(PxExtendedVec3(pos.X, pos.Y + 1.0f, pos.Z));
}

void CCTSubsystem::onEntityRemoved(anax::Entity& entity)
{
    entity.getComponent<CCTComponent>().controller->release();
}

void CCTSubsystem::init(PxScene* scene)
{
    m_capsuleDesc.height = 2.0f * IRR_PHYSX_DIM_SCALAR;
    m_capsuleDesc.radius = 0.5f * IRR_PHYSX_DIM_SCALAR;
    m_capsuleDesc.scaleCoeff = 0.8f;
    m_capsuleDesc.volumeGrowth = 1.5f;
    m_capsuleDesc.density = 10.0f;
    m_capsuleDesc.slopeLimit = .65f;
    m_capsuleDesc.stepOffset = .5f * IRR_PHYSX_DIM_SCALAR;
    m_capsuleDesc.contactOffset = .1f;
    m_capsuleDesc.material = PhysicsManager::Get()->getDefaultMaterial();
}

void CCTSubsystem::update(float dt)
{
    /*PxControllerFilters filters;

    auto& entities = getEntities();

    for (auto& entity : entities) 
    {
        auto controller = entity.getComponent<CCTComponent>().controller;
		
        auto collisionFlags = controller->move(
                entity.getComponent<CCTComponent>().displacement,
                0.1f,
                dt,
                filters);

        entity.getComponent<TransformComponent>().setPosition(vector3df(
            static_cast<float>(controller->getPosition().x),
			static_cast<float>(controller->getPosition().y),
			static_cast<float>(controller->getPosition().z)));
    }*/
}

void CCTSubsystem::cleanup()
{
   
}
