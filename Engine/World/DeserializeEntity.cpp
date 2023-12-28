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

entityid WorldManager::deserializeEntity(const string& file, entityid id, bool use_saved_transform, const string& name,
                                         const irr::core::vector3df& position, const irr::core::vector3df& rotation,
                                         const irr::core::vector3df& scale)
{
    Entity entity;

    auto xml = new XMLDocument;

    if (xml->LoadFile(file.c_str()) != XML_NO_ERROR)
    {
        spdlog::warn("Failed to load entity \'" + file + "\' in WorldManager::deserializeBaseEntity()");

        delete xml;
        xml = nullptr;

        return _entity_null_value;
    }

    try
    {
        auto root = xml->FirstChild()->NextSibling();
    	
        for (
            XMLNode* entityNode = root->FirstChildElement();
            entityNode != nullptr;
            entityNode = entityNode->NextSiblingElement())
        {
        	// Load scene description if present
			if (string(entityNode->Value()) == "scene")
			{
				SceneDescriptor scene_desc;
				
				XMLElement* sceneNode = entityNode->FirstChildElement()->FirstChildElement();
				for (;
					sceneNode != nullptr;
					sceneNode = sceneNode->NextSiblingElement())
				{
					if (string(sceneNode->Name()) == "name")
					{
						if (sceneNode->GetText())
							scene_desc.name = sceneNode->GetText();
					}

					if (string(sceneNode->Name()) == "creator")
					{
						if (sceneNode->GetText())
							scene_desc.creator = sceneNode->GetText();
					}

					if (string(sceneNode->Name()) == "notes")
					{
						if (sceneNode->GetText())
							scene_desc.notes = sceneNode->GetText();
					}

					if (string(sceneNode->Name()) == "skydome_texture")
					{
						if (sceneNode->GetText())
							scene_desc.skydome_texture = sceneNode->GetText();
					}

					if (string(sceneNode->Name()) == "ambient_light.r")
					{
						scene_desc.ambient_light.r = static_cast<float>(atof(sceneNode->GetText()));
					}
					if (string(sceneNode->Name()) == "ambient_light.g")
					{
						scene_desc.ambient_light.g = static_cast<float>(atof(sceneNode->GetText()));
					}
					if (string(sceneNode->Name()) == "ambient_light.b")
					{
						scene_desc.ambient_light.b = static_cast<float>(atof(sceneNode->GetText()));
					}
				}

				m_currentSceneDescriptor = scene_desc;

				continue;
			}

        	// TODO: Placeholder
			// NOIMP
			// Load prefab description if present
			if (string(entityNode->Value()) == "prefab")
			{
				XMLElement* prefabNode = entityNode->FirstChildElement()->FirstChildElement();
				for (;
					prefabNode != nullptr;
					prefabNode = prefabNode->NextSiblingElement())
				{
					/*if (string(prefabNode->Name()) == "name")
					{
						prefab.name = prefabNode->GetText();
					}*/
				}
				
				continue;
			}
        	
            entity = m_gameWorld.createEntity();

            XMLNode* component = entityNode->FirstChildElement();
            for (;
                component != nullptr;
                component = component->NextSiblingElement())
            {
                if (string(component->Value()) == "descriptor")
                {
                    entity.addComponent<DescriptorComponent>();

                    if (id < _entity_null_value) { entity.getComponent<DescriptorComponent>().id = id; }
                    else
                    {
                        bool flag = true;
                        for (auto i = 0U; i < _entity_null_value; i++)
                        {
                            if (!m_entityIDArray[i])
                            {
                                entity.getComponent<DescriptorComponent>().id = i;

                                m_entityIDArray[i] = true;
                                flag = false;

                                break;
                            }
                        }

                        if (flag)
                        {
                            spdlog::warn(
                                "World reports entity count exceeded entity_null_value " + std::to_string(
                                    _entity_null_value));
                        }
                    }

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (std::string(value->Name()) == "name")
                        {
                            if (name.empty())
                            {
                                entity.getComponent<DescriptorComponent>().name =
                                    value->GetText();
                            }
                            else { entity.getComponent<DescriptorComponent>().name = name; }
                        }
                        if (std::string(value->Name()) == "type")
                        {
                            entity.getComponent<DescriptorComponent>().type =
                                static_cast<ENTITY_TYPE>(atoi(value->GetText()));
                        }
                        if (std::string(value->Name()) == "isSerializable")
                        {
                            entity.getComponent<DescriptorComponent>().isSerializable =
                                Utility::ProcessBoolStatement(std::string(value->GetText()));
                        }
                        if (std::string(value->Name()) == "isdebug")
                        {
                            entity.getComponent<DescriptorComponent>().isDebug =
                                Utility::EvalTrueFalse(value->GetText());
                        }
                    }
                }

				if (string(component->Value()) == "transform")
				{
					entity.addComponent<TransformComponent>();

					if (!use_saved_transform)
					{
						// *** DEPRECATED ***
						/*
						entity.getComponent<TransformComponent>().position = position;
						entity.getComponent<TransformComponent>().rotation = rotation;
						entity.getComponent<TransformComponent>().scale    = scale;
						*/

						entity.getComponent<TransformComponent>().setPosition(position);
						entity.getComponent<TransformComponent>().setRotation(rotation);
						entity.getComponent<TransformComponent>().setScale(scale);

						entity.getComponent<TransformComponent>().initialPosition = position;
						entity.getComponent<TransformComponent>().initialRotation = rotation;
						entity.getComponent<TransformComponent>().initialScale = scale;
					}
					else
					{
						auto iter = 0;
						float val[18] = {
							0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
							1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
						};

						auto value = component->FirstChild()->FirstChildElement();
						for (;
							value != nullptr;
							value = value->NextSiblingElement())
						{
							if (iter > 17) { break; }

							val[iter++] = static_cast<float>(atof(value->GetText()));
						}

						/* *** DEPRECATED ***
						entity.getComponent<TransformComponent>().position =
							irr::core::vector3df(val[0], val[1], val[2]);
						entity.getComponent<TransformComponent>().rotation =
							irr::core::vector3df(val[3], val[4], val[5]);
						entity.getComponent<TransformComponent>().scale    =
							irr::core::vector3df(val[6], val[7], val[8]);
						*/

						entity.getComponent<TransformComponent>().setPosition(
							irr::core::vector3df(val[0], val[1], val[2]));
						entity.getComponent<TransformComponent>().setRotation(
							irr::core::vector3df(val[3], val[4], val[5]));
						entity.getComponent<TransformComponent>().setScale(
							irr::core::vector3df(val[6], val[7], val[8]));

						entity.getComponent<TransformComponent>().initialPosition =
							irr::core::vector3df(val[9], val[10], val[11]);
						entity.getComponent<TransformComponent>().initialRotation =
							irr::core::vector3df(val[12], val[13], val[14]);
						entity.getComponent<TransformComponent>().initialScale =
							irr::core::vector3df(val[15], val[16], val[17]);
					}

					auto value = component->FirstChild()->FirstChildElement();
					for (;
						value != nullptr;
						value = value->NextSiblingElement())
					{
						if (std::string(value->Name()) == "isParent")
						{
							entity.getComponent<TransformComponent>().isParent = Utility::EvalTrueFalse(value->GetText());
						}

						if (std::string(value->Name()) == "isChild")
						{
							entity.getComponent<TransformComponent>().isChild = Utility::EvalTrueFalse(value->GetText());
						}

						if (std::string(value->Name()) == "parent_name")
						{
							entity.getComponent<TransformComponent>().parent_name = std::string(value->GetText());
						}

						if (std::string(value->Name()) == "children_names")
						{
							auto subvalue = value->FirstChildElement();
							for (;
								subvalue != nullptr;
								subvalue = subvalue->NextSiblingElement())
							{
								entity.getComponent<TransformComponent>().children_names.push_back(string(subvalue->GetText()));
							}
						}
					}
				}

                if (string(component->Value()) == "script")
                {
                    entity.addComponent<ScriptComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (string(value->Name()) == "script" || string(value->Name()) == "file")
                        {
                            entity.getComponent<ScriptComponent>().script = value->GetText();

                            // The system adds a unique module identifier
                            //entity.getComponent<ScriptComponent>().module = value->GetText();
                        }

                        if (string(value->Name()) == "data")
                        {
                            auto subvalue = value->FirstChildElement();
                            for (;
                                subvalue != nullptr;
                                subvalue = subvalue->NextSiblingElement())
                            {
                                std::string script_data = subvalue->GetText();

                                entity.getComponent<ScriptComponent>().script_data.emplace_back(ExposedScriptData(
                                    atoi(script_data.substr(0, 1).c_str()),
                                    "",
                                    static_cast<AS_DATA_TYPE>(atoi(script_data.substr(2, 1).c_str())),
                                    script_data.substr(4)));
                            }
                        }
                    }
                }

                if (string(component->Value()) == "render")
                {
                    entity.addComponent<RenderComponent>().isVisible = Utility::EvalTrueFalse(
                        component->FirstChild()->FirstChildElement()->GetText());
                }

                if (string(component->Value()) == "mesh")
                {
                    entity.addComponent<MeshComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (string(value->Name()) == "mesh")
                        {
                            entity.getComponent<MeshComponent>().mesh = value->GetText();
                        }

                        if (string(value->Name()) == "isAnimated")
                        {
                            entity.getComponent<MeshComponent>().isAnimated = Utility::EvalTrueFalse(value->GetText());
                        }

                        if (string(value->Name()) == "castShadows")
                        {
                            entity.getComponent<MeshComponent>().castShadows = Utility::EvalTrueFalse(value->GetText());
                        }

                        if (string(value->Name()) == "receiveShadows")
                        {
                            entity.getComponent<MeshComponent>().receiveShadows = Utility::EvalTrueFalse(
                                value->GetText());
                        }

						if (string(value->Name()) == "transparent")
						{
							entity.getComponent<MeshComponent>().transparent = Utility::
								EvalTrueFalse(value->GetText());
						}

                        if (string(value->Name()) == "disableZDraw")
                        {
                            entity.getComponent<MeshComponent>().disableZDraw = Utility::
                                EvalTrueFalse(value->GetText());
                        }

						if (string(value->Name()) == "disableDeferredRendering")
						{
							entity.getComponent<MeshComponent>().disableDeferredRendering = Utility::
								EvalTrueFalse(value->GetText());
						}

                        if (string(value->Name()) == "renderMaterial")
                        {
                            entity.getComponent<MeshComponent>().renderMaterial = static_cast<irr::video::
                                E_MATERIAL_TYPE>(atoi(value->GetText()));
                        }

                        if (string(value->Name()) == "textures")
                        {
                            auto subvalue = value->FirstChildElement();
                            for (;
                                subvalue != nullptr;
                                subvalue = subvalue->NextSiblingElement())
                            {
                                entity.getComponent<MeshComponent>().textures.push_back(string(subvalue->GetText()));
                            }
                        }

                        // DEPRECATED, Load anims from seperate file
                        /*if (string(value->Name()) == "animationList") {
                            auto subvalue = value->FirstChildElement();
                            for (;
                                subvalue != nullptr;
                                subvalue = subvalue->NextSiblingElement()) {
                                string frames = subvalue->GetText(),
                                    animname = subvalue->Attribute("name");

                                auto loop = Utility::ProcessBoolStatement(string(subvalue->Attribute("loop")));

                                entity.getComponent<MeshComponent>().animationList.push_back(
                                    sAnimationData(
                                        animname,
                                        stoi(frames.substr(0, frames.find_first_of(','))),
                                        stoi(frames.substr(frames.find_first_of(',') + 1)),
                                        loop));
                            }
                        }*/
                    }

                    if (entity.getComponent<MeshComponent>().isAnimated)
                    {
                        auto anim_xml_name = entity.getComponent<MeshComponent>().mesh;
                    	
                        XMLDocument anim_xml;
                    	
                        if (anim_xml.
                            LoadFile(
                                std::string(anim_xml_name.substr(0, anim_xml_name.find_last_of('.') + 1) + "anim").
                                c_str()) != XML_NO_ERROR)
                        {
                            spdlog::warn("Failed to load animation data \'" + std::string(
                                anim_xml_name.substr(0, anim_xml_name.find_last_of('.') + 1) + "anim") + "\' ");
                        	
                            continue;
                        }

                        auto anim_root  = anim_xml.FirstChild()->NextSibling();
                        auto anim_value = anim_root->FirstChildElement();
                    	
                        for (;
                            anim_value != nullptr;
                            anim_value = anim_value->NextSiblingElement())
                        {
                            if (std::string(anim_value->Name()) == "fps")
                            {
                                entity.getComponent<MeshComponent>().fps = static_cast<unsigned int>(atoi(
                                    anim_value->GetText()));
                            }

                            if (std::string(anim_value->Name()) == "animationList")
                            {
                                auto anim_subvalue = anim_value->FirstChildElement();
                            	
                                for (;
                                    anim_subvalue != nullptr;
                                    anim_subvalue = anim_subvalue->NextSiblingElement())
                                {
                                    string frames   = anim_subvalue->GetText(),
                                           animname = anim_subvalue->Attribute("name");

                                    auto loop = Utility::ProcessBoolStatement(string(anim_subvalue->Attribute("loop")));

                                    entity.getComponent<MeshComponent>().animationList.push_back(
                                        sAnimationData(
                                            animname,
                                            stoi(frames.substr(0, frames.find_first_of(','))),
                                            stoi(frames.substr(frames.find_first_of(',') + 1)),
                                            loop));
                                }
                            }
                        }

                        anim_xml.Clear();
                    }
                }

                if (string(component->Value()) == "camera")
                {
                    auto iter = 0;
                    float val[6] = {0, 0, 0, 0, 0, 0};

                    entity.addComponent<CameraComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        val[iter++] = static_cast<float>(atof(value->GetText()));
                    }
                    entity.getComponent<CameraComponent>().offset =
                        irr::core::vector3df(val[0], val[1], val[2]);
                    entity.getComponent<CameraComponent>().target =
                        irr::core::vector3df(val[3], val[4], val[5]);
                }

                if (string(component->Value()) == "debugsprite")
                {
                    entity.addComponent<DebugSpriteComponent>().sprite = component
                                                                         ->FirstChild()->FirstChildElement()->GetText();
                }

                if (string(component->Value()) == "soundlistener") { entity.addComponent<SoundListenerComponent>(); }

                if (string(component->Value()) == "light")
                {
                    entity.addComponent<LightComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (string(value->Name()) == "type")
                        {
                            entity.getComponent<LightComponent>().type = static_cast<LIGHT_TYPE>(atoi(value->GetText())
                            );
                        }
                        if (string(value->Name()) == "visible")
                        {
                            entity.getComponent<LightComponent>().visible = Utility::EvalTrueFalse(value->GetText());
                        }
                        if (string(value->Name()) == "radius")
                        {
                            entity.getComponent<LightComponent>().radius = static_cast<float>(atof(value->GetText()));
                        }
                        if (string(value->Name()) == "outerCone")
                        {
                            entity.getComponent<LightComponent>().outerCone = static_cast<float>(atof(value->GetText())
                            );
                        }
                        if (string(value->Name()) == "innerCone")
                        {
                            entity.getComponent<LightComponent>().innerCone = static_cast<float>(atof(value->GetText())
                            );
                        }
                        if (string(value->Name()) == "falloff")
                        {
                            entity.getComponent<LightComponent>().falloff = static_cast<float>(atof(value->GetText()));
                        }
                        if (string(value->Name()) == "color_diffuse.r")
                        {
                            entity.getComponent<LightComponent>().color_diffuse.r = static_cast<float>(atof(
                                value->GetText()));
                        }
                        if (string(value->Name()) == "color_diffuse.g")
                        {
                            entity.getComponent<LightComponent>().color_diffuse.g = static_cast<float>(atof(
                                value->GetText()));
                        }
                        if (string(value->Name()) == "color_diffuse.b")
                        {
                            entity.getComponent<LightComponent>().color_diffuse.b = static_cast<float>(atof(
                                value->GetText()));
                        }
                        if (string(value->Name()) == "offset.X")
                        {
                            entity.getComponent<LightComponent>().offset.X = static_cast<float>(atof(value->GetText()));
                        }
                        if (string(value->Name()) == "offset.Y")
                        {
                            entity.getComponent<LightComponent>().offset.Y = static_cast<float>(atof(value->GetText()));
                        }
                        if (string(value->Name()) == "offset.Z")
                        {
                            entity.getComponent<LightComponent>().offset.Z = static_cast<float>(atof(value->GetText()));
                        }
                    }
                }

                if (string(component->Value()) == "cct")
                {
                    entity.addComponent<CCTComponent>();
                }

                if (string(component->Value()) == "billboard")
                {
                    entity.addComponent<BillboardSpriteComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (string(value->Name()) == "sprite")
                        {
                            entity.getComponent<BillboardSpriteComponent>().sprite = value->GetText();
                        }

                        if (string(value->Name()) == "split_x")
                        {
                            entity.getComponent<BillboardSpriteComponent>().split_x = atoi(value->GetText());
                        }

                        if (string(value->Name()) == "split_y")
                        {
                            entity.getComponent<BillboardSpriteComponent>().split_y = atoi(value->GetText());
                        }

                        if (string(value->Name()) == "fps")
                        {
                            entity.getComponent<BillboardSpriteComponent>().fps = atoi(value->GetText());
                        }

                        if (string(value->Name()) == "animated")
                        {
                            entity.getComponent<BillboardSpriteComponent>().animated = Utility::ProcessBoolStatement(
                                value->GetText());
                        }

						if (string(value->Name()) == "destroyOnFinish")
						{
							entity.getComponent<BillboardSpriteComponent>().destroyOnFinish = Utility::ProcessBoolStatement(
								value->GetText());
						}

                        if (string(value->Name()) == "scale_x")
                        {
                            entity.getComponent<BillboardSpriteComponent>().scale_x = static_cast<float>(atof(value->GetText()));
                        }

                        if (string(value->Name()) == "scale_y")
                        {
                            entity.getComponent<BillboardSpriteComponent>().scale_y = static_cast<float>(atof(value->GetText()));
                        }

                        if (string(value->Name()) == "loop")
                        {
                            entity.getComponent<BillboardSpriteComponent>().loop = Utility::ProcessBoolStatement(
                                value->GetText());
                        }

                        if (string(value->Name()) == "finished")
                        {
                            entity.getComponent<BillboardSpriteComponent>().finished = Utility::ProcessBoolStatement(
                                value->GetText());
                        }

                        if (string(value->Name()) == "finished")
                        {
                            entity.getComponent<BillboardSpriteComponent>().finished = atoi(value->GetText());
                        }
                    }
                }

                if (string(component->Value()) == "debugmesh")
                {
                    entity.addComponent<DebugMeshComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (string(value->Name()) == "mesh")
                        {
                            entity.getComponent<DebugMeshComponent>().mesh = value->GetText();
                        }
                        if (string(value->Name()) == "texture")
                        {
                            entity.getComponent<DebugMeshComponent>().texture = value->GetText();
                        }
                    }
                }

                if (string(component->Value()) == "physics") {
                    entity.addComponent<PhysicsComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement()) {

                        if (string(value->Name()) == "type") {
                            entity.getComponent<PhysicsComponent>().type = static_cast<PHYSICS_COLLIDER_TYPE>(atoi(value->GetText()));
                        }

                        if (string(value->Name()) == "kinematic") {
                            entity.getComponent<PhysicsComponent>().kinematic = ::Utility::EvalTrueFalse(value->GetText());
                        }

                        if (string(value->Name()) == "collisionMesh") {
                            // BUG: FIXED. Need to make sure value->GetText() is not nullptr if XML tag is empty, many occurences in *deserialize to fix...
                            if (value->GetText()) {
                                entity.getComponent<PhysicsComponent>().collisionMesh = value->GetText();
                            }
                        }

                        if (string(value->Name()) == "density") {
                            entity.getComponent<PhysicsComponent>().density = static_cast<float>(atof(value->GetText()));
                        }
                    }
                }

                if (string(component->Value()) == "sound")
                {
                    entity.addComponent<SoundComponent>();

                    auto value = component->FirstChild()->FirstChildElement();
                    for (;
                        value != nullptr;
                        value = value->NextSiblingElement())
                    {
                        if (std::string(value->Name()) == "s.name")
                        {
                            auto sname = string(value->GetText());
                            std::string sfile = "";
                            bool is3D = false;
                            bool loop = false;
                            bool startpaused = true;
                            float volume = 100;
                            float minDist = 1.0;

                            auto subvalue = value->NextSiblingElement();
                            for (int i = 0;
                                 i < 6;
                                 subvalue = subvalue->NextSiblingElement(), i++)
                            {
                                if (std::string(subvalue->Name()) == "s.file")
                                {
                                    sfile = std::string(subvalue->GetText());
                                }

                                if (std::string(subvalue->Name()) == "s.is3D")
                                {
                                    is3D = Utility::ProcessBoolStatement(string(subvalue->GetText()));
                                }

                                if (std::string(subvalue->Name()) == "s.loop")
                                {
                                    loop = Utility::ProcessBoolStatement(string(subvalue->GetText()));
                                }

                                if (std::string(subvalue->Name()) == "s.startPaused")
                                {
                                    startpaused = Utility::ProcessBoolStatement(string(subvalue->GetText()));
                                }

                                if (std::string(subvalue->Name()) == "s.volume")
                                {
                                    volume = static_cast<float>(atof(string(subvalue->GetText()).c_str()));
                                }

                                if (std::string(subvalue->Name()) == "s.minDist")
                                {
                                    minDist = static_cast<float>(atof(string(subvalue->GetText()).c_str()));
                                }
                            }

                            entity.getComponent<SoundComponent>().sounds.emplace_back(
                                sSoundData(sfile, sname, is3D, loop, volume, minDist, 1,
                                           startpaused));
                        }
                    }
                }

                GameState::deserializeComponent(entity, component);
            }

            entity.activate();
            m_gameWorld.refresh();
        }
    }
    catch (...)
    {
        spdlog::warn("Failed to deserialize entity file: " + file + " in WorldManager::deserializeEntity()");
    }

    delete xml;
    xml = nullptr;

    if (entity.isValid())
    {
        if (entity.hasComponent<DescriptorComponent>()) { return entity.getComponent<DescriptorComponent>().id; }

        spdlog::warn(
            "Failed to deserialize entity file: " + file +
            " in WorldManager::deserializeWorld(), no descriptor component found");
        return _entity_null_value;
    }

    spdlog::warn("Failed to deserialize entity file: " + file + " in WorldManager::deserializeWorld(), entity invalid");

    return _entity_null_value;
}
