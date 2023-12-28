#include "Engine/Renderer/RenderManager.h"

#include <fstream>

#include <cereal/archives/xml.hpp>
#include <IMGUI/imgui.h>
#include <spdlog/spdlog.h>

#include "Engine/Engine.h"
#include "Utility/Utility.h"

using namespace std;
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

RenderManager* RenderManager::s_Instance = nullptr;
irr::gui::IGUIFont* g_DefaultTextRenderableFontSm;

void Set_IMGUI_Default_Theme()
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.898f, 0.850f, 0.858f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.498f, 0.450f, 0.458f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.6f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.2f, 0.3f, 0.6f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.13f, 0.4f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.13f, 0.4f, 0.8f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.13f, 0.4f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.13f, 0.4f, 0.8f, 1.0f));
	
    ImGui::PushStyleColor(ImGuiCol_TooltipBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().ChildWindowRounding = 0.0f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;
}

void ShaderMaterialManager::add(ShaderMaterial material)
{
    s_ShaderMaterialList.push_back(material);
}

E_MATERIAL_TYPE ShaderMaterialManager::get(std::string name)
{
    for (auto m : s_ShaderMaterialList) {
        if (m.name == name) {
            return static_cast<E_MATERIAL_TYPE>(m.material);
        }
    }

    spdlog::warn("ShaderMaterialManager: Shader material {} invalid", name);

    return EMT_SOLID;
}

vector<ShaderMaterial> ShaderMaterialManager::s_ShaderMaterialList;

void ShaderConstantSetCallBack::OnSetConstants(IMaterialRendererServices* services, s32 userData)
{
    const auto driver = services->getVideoDriver();

    auto time = static_cast<f32>(Engine::Get()->getCurrentTime());
    services->setVertexShaderConstant("fTime", &time, 1);

    auto resolution = vector2df(static_cast<irr::f32>(RenderManager::Get()->driver()->getScreenSize().Width), static_cast<irr::f32>(RenderManager::Get()->driver()->getScreenSize().Height));
    services->setVertexShaderConstant("iResolution", reinterpret_cast<f32*>(&resolution), 2);

    auto invWorld = driver->getTransform(ETS_WORLD);
    invWorld.makeInverse();

    services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

    auto worldViewProj = driver->getTransform(ETS_PROJECTION);
    worldViewProj *= driver->getTransform(ETS_VIEW);
    worldViewProj *= driver->getTransform(ETS_WORLD);

    services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

    auto world = driver->getTransform(ETS_WORLD);
    world = world.getTransposed();

    services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

    auto pos = RenderManager::Get()->sceneManager()->getActiveCamera()->getAbsolutePosition();

    services->setVertexShaderConstant("fCameraPos", reinterpret_cast<f32*>(&pos), 3);

    auto textureLayerID = 0;
    services->setPixelShaderConstant("tDiffuse", &textureLayerID, 1);
}

RenderManager::RenderManager(const std::string& name, const std::string& args) :
    m_drawDebugStatistics(false),
	m_defaultCamera(nullptr),
    m_device(nullptr),
    m_gui(nullptr),
    m_driver(nullptr),
    m_gpu(nullptr),
    m_sceneManager(nullptr),
    m_renderer(nullptr),
	m_effectManager(nullptr),
    m_shaderConstantCallBack(nullptr),
    m_imgui(nullptr),
    m_backgroundColor(255, 37, 37, 37)
{
    if (s_Instance)
    {
        Utility::Error("Pointer to class \'RenderManager\' is invalid");
    }
    s_Instance = this;

    try
    {
        std::ifstream ifs_render("config/render.xml");
        cereal::XMLInputArchive render_config(ifs_render);

        render_config(m_configuration);
    }
    catch (cereal::Exception& ex)
    {
        spdlog::warn("Failed to load render configuration: {}, default values used", ex.what());

        m_configuration = RenderConfiguration();

        std::ofstream ofs_render("config/render.xml");
        cereal::XMLOutputArchive render_config(ofs_render);

        render_config(m_configuration);
    }

	if (!Utility::GetCmdlOptionExists(args, "editor") || m_configuration.mode >= 0)
	{
		getResolutionFromMode(m_configuration.aspect, m_configuration.mode, m_configuration.width, m_configuration.height);
	}

    m_irrlichtParams.WindowId = nullptr;

	if (!m_configuration.fullscreen &&
		Utility::GetCmdlOptionExists(args, "editor"))
	{
		m_irrlichtParams.WindowSize = dimension2d<u32>(m_configuration.editor_width, m_configuration.editor_height);
	}
	else
	{
		m_irrlichtParams.WindowSize = dimension2d<u32>(m_configuration.width, m_configuration.height);
	}

    m_irrlichtParams.Bits = 32;
    m_irrlichtParams.Fullscreen = m_configuration.fullscreen;
    m_irrlichtParams.Stencilbuffer = false;
    m_irrlichtParams.AntiAlias = m_configuration.antialiasingFactor;
    m_irrlichtParams.Vsync = m_configuration.vSync;
    m_irrlichtParams.EventReceiver = &m_imguiEventReceiver;
    m_irrlichtParams.ZBufferBits = 32;
    m_irrlichtParams.LoggingLevel = ELL_NONE;

    if (m_configuration.api)
    {
        m_irrlichtParams.DriverType = EDT_DIRECT3D9;

        spdlog::info("Irrlicht Version {} using API Direct3D 9", IRRLICHT_SDK_VERSION);
    }
    else
    {
        m_irrlichtParams.DriverType = EDT_OPENGL;

        spdlog::info("Irrlicht Version {} using API OpenGL", IRRLICHT_SDK_VERSION);
    }

    m_device = createDeviceEx(m_irrlichtParams);
    if (!m_device)
    {
        spdlog::error("Failed to create the rendering device");
    }

    m_driver = m_device->getVideoDriver();
    if (!m_driver)
    {
        spdlog::error("Failed to initialize the graphics API");
    }

	m_driver->setAllowZWriteOnTransparent(true);

    m_sceneManager = m_device->getSceneManager();
    if (!m_sceneManager)
    {
        spdlog::error("Failed to create the scene manager");
    }

    m_gpu = m_driver->getGPUProgrammingServices();
    if (!m_gpu)
    {
        spdlog::error("Failed to register GPU programming services");
    }

    m_shaderConstantCallBack = new ShaderConstantSetCallBack;
    if (!m_shaderConstantCallBack)
    {
        spdlog::error("Failed to create the ShaderConstantSetCallBack interface");
    }

    m_gui = m_device->getGUIEnvironment();
    if (!m_gui)
    {
        spdlog::error("Failed to create the GUI renderer");
    }

	if (!m_configuration.fullscreen && 
		Utility::GetCmdlOptionExists(args, "editor"))
	{
		// BUG: Resizing window after 1 or 2 resizes crashes program, unknown reason
		m_device->maximizeWindow();
		m_device->setResizable(true);
	}

    m_renderer = CRenderer::createRenderer(m_device);
    if (!m_renderer)
    {
        spdlog::error("Failed to create the deferred scene renderer");
    }

	/*m_effectManager = new EffectHandler(m_device, dimension2du(m_configuration.width, m_configuration.height), false, true, false);
	if (!m_effectManager)
	{
		spdlog::error("Failed to create the effect manager");
	}
	m_effectManager->setAmbientColor(SColor(255, 32, 32, 32));
	m_effectManager->setClearColour(SColor(255, 32, 32, 32));*/

    m_imgui = createIMGUI(m_device, &m_imguiEventReceiver);
    if (!m_imgui) {
        spdlog::error("Failed to create the IMGUI renderer");
    }
    Set_IMGUI_Default_Theme();

	// DPI scaling for ImGui
	if (m_configuration.width > 2550 && m_configuration.height > 1440)
	{
		m_configuration.dpi_scale = 2;
	}
	else
	{
		m_configuration.dpi_scale = 1;
	}
	

	ImGui::GetIO().FontGlobalScale = static_cast<float>(m_configuration.dpi_scale);
	ImGui::GetStyle().ScaleAllSizes(static_cast<float>(m_configuration.dpi_scale));

    m_driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);

    stringw windowName = name.c_str();
    m_device->setWindowCaption(windowName.c_str());

    m_sceneManager->setAmbientLight(SColor(255, 255, 255, 255));

    m_sceneManager->getParameters()->setAttribute(ALLOW_ZWRITE_ON_TRANSPARENT, true);

    m_driver->getMaterial2D().TextureLayer[0].BilinearFilter = true;
    m_driver->getMaterial2D().TextureLayer[0].TrilinearFilter = true;
    m_driver->getMaterial2D().AntiAliasing = EAAM_SIMPLE;

    createDefaultShaders();

    g_DefaultTextRenderableFontSm = m_device->getGUIEnvironment()->getFont("content/texture/font/defaultfont_sm.bmp");

    initDefaultSkyDome();

	// Make a default camera
	m_defaultCamera = m_sceneManager->addCameraSceneNode();

	m_device->getCursorControl()->setVisible(false);
	
}
RenderManager::~RenderManager()
{
    m_device->closeDevice();

	delete m_effectManager;

    delete s_Instance;
}

void RenderManager::beginImGui()
{
    m_imgui->startGUI();
}

void RenderManager::draw(f32 dt)
{
	// BUG: Can only resize 2 or 3 times before crash, see "CRenderer.cpp" line 129
    if (m_driver->getScreenSize().Height != m_configuration.height || 
        m_driver->getScreenSize().Width  != m_configuration.width)
    {
        m_configuration.width  = m_driver->getScreenSize().Width;
        m_configuration.height = m_driver->getScreenSize().Height;

		if (m_sceneManager->getActiveCamera()) {
			//m_sceneManager->getActiveCamera()->setAspectRatio(static_cast<float>(m_configuration.width) / static_cast<float>(m_configuration.height));
		}

		//m_renderer->resetMRTs();
    }

    m_driver->beginScene(true, true, m_backgroundColor);
	
    m_sceneManager->drawAll();
	//m_effectManager->update();

    for (auto line : m_lineRenderableList) 
    {
        SMaterial mtl;
        mtl.Lighting = false;

        m_driver->setMaterial(mtl);
        m_driver->setTransform(ETS_WORLD, IdentityMatrix);

        m_driver->draw3DLine(line.line.start, line.line.end, line.color);
    }
    m_lineRenderableList.clear();

	m_driver->enableMaterial2D();
	{
		for (const auto& image : m_imageRenderableList) {
			if (!image.image) {
				continue;
			}

			auto img_rect = irr::core::rect<s32>(0, 0, image.image->getSize().Width, image.image->getSize().Height);

			m_driver->draw2DImage(image.image, image.position, img_rect, nullptr, image.color, image.use_alpha);
		}
	}
	m_driver->enableMaterial2D(false);

	for (auto text : m_textRenderableList) {
		text.font->draw(text.text, text.position, text.color, text.hcenter, text.vcenter);
	}

    m_imgui->drawAll();

	//m_driver->runAllOcclusionQueries(false);
	//m_driver->updateAllOcclusionQueries();

    m_driver->endScene();

	m_textRenderableList.clear();
	//vector<Text2D>().swap(m_textRenderableList);
	m_imageRenderableList.clear();
	//vector<Image2D>().swap(m_imageRenderableList);
	m_lineRenderableList.clear();
}

void RenderManager::createShaderMaterial(
    std::string name, std::string fragment, std::string vertex,
    E_MATERIAL_TYPE materialType, E_PIXEL_SHADER_TYPE psVersion, E_VERTEX_SHADER_TYPE vsVersion)
{
    ShaderMaterial shader(name);

    shader.material = m_gpu->addHighLevelShaderMaterialFromFiles(
        vertex.c_str(), "main", vsVersion,
        fragment.c_str(), "main", psVersion,
        m_shaderConstantCallBack, materialType, 0, EGSL_DEFAULT);

    ShaderMaterialManager::add(shader);
}

void RenderManager::createDefaultShaders()
{
    createShaderMaterial("unlit", "content/shader/unlit.frag", "content/shader/unlit.vert");
    createShaderMaterial("phong", "content/shader/phong.frag", "content/shader/phong.vert");
    createShaderMaterial("varicolor", "content/shader/varicolor.frag", "content/shader/unlit.vert");
}

void RenderManager::initDefaultSkyDome(std::string texture)
{
    m_currentSkydomeTexture = texture;

    m_driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);

    m_defaultSkyDome = m_sceneManager->addSkyDomeSceneNode(
        m_driver->getTexture(texture.c_str()), 32, 16);

    m_driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);
}

void RenderManager::removeDefaultSkyDome()
{
    if (m_defaultSkyDome) {
        m_defaultSkyDome->remove();
        m_currentSkydomeTexture = std::string();
    }
}

void RenderManager::swapSkyDomeTexture(std::string texture)
{
    m_currentSkydomeTexture = texture;

    if (m_defaultSkyDome) 
	{
		m_driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
        m_defaultSkyDome->setMaterialTexture(0, m_driver->getTexture(texture.c_str()));
		m_driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);
    }
}

ISceneNode *RenderManager::getNodeFromCursorPosition(ICameraSceneNode *camera)
{
    line3d<f32> ray = m_sceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(
        m_device->getCursorControl()->getPosition(), camera ? camera : m_sceneManager->getActiveCamera());

    vector3df point;
    triangle3df triangle;
	
    auto node =
        m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, triangle, 0, nullptr, true);

	return node;
}

vector3df RenderManager::getPoint3DFromCursorPosition(ICameraSceneNode *camera)
{
    line3d<f32> ray = m_sceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(
        m_device->getCursorControl()->getPosition(), camera ? camera : m_sceneManager->getActiveCamera());

    vector3df point;
    triangle3df triangle;

    m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, triangle);

    return point;
}

ISceneNode* RenderManager::getNodeFromRaycast(vector3df start, vector3df end)
{
    line3df ray;
    ray.start = start;
    ray.end = end;

    vector3df point;
    triangle3df tri;

    ISceneNode* selectedNode = 0;
    selectedNode = m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, tri);

    if (!selectedNode) {
        return nullptr;
    }

    return selectedNode;
}

bool RenderManager::getNodeTriangleTextureName(ISceneNode* node, const triangle3df& tri, std::string& texname)
{
    IMesh* mesh = 0;

    ESCENE_NODE_TYPE type = node->getType();
    if (type == ESNT_MESH || type == ESNT_OCTREE) {
        mesh = dynamic_cast<IMeshSceneNode*>(node)->getMesh();
    }
    else
        if (type == ESNT_ANIMATED_MESH) {
            mesh = dynamic_cast<IAnimatedMeshSceneNode*>(node)->getMesh()->getMesh(0);
        }
        else {
            return false;
        }

    if (!mesh) {
        return false;
    }

    vector3df ptA = tri.pointA;
    vector3df ptB = tri.pointB;
    vector3df ptC = tri.pointC;

    matrix4 matrix = node->getAbsoluteTransformation();
    matrix4 inverse;
    vector3df p0, p1, p2;

    if (matrix.getInverse(inverse)) {
        inverse.transformVect(p0, ptA);
        inverse.transformVect(p1, ptB);
        inverse.transformVect(p2, ptC);
    }
    else {
        p0 = ptA; p1 = ptB; p2 = ptC;
    }

    for (u32 i = 0; i < mesh->getMeshBufferCount(); ++i)
    {
        bool p0Found = false;
        bool p1Found = false;
        bool p2Found = false;

        IMeshBuffer* buf = mesh->getMeshBuffer(i);
        for (u32 j = 0; j < buf->getVertexCount(); ++j) {
            vector3df pos = buf->getPosition(j);

            if ((!p0Found) && (pos.equals(p0))) {
                p0Found = true;
            }

            if ((!p1Found) && (pos.equals(p1))) {
                p1Found = true;
            }

            if ((!p2Found) && (pos.equals(p2))) {
                p2Found = true;
            }
        }

        if (p0Found && p1Found && p2Found) {
            ITexture* tex = buf->getMaterial().getTexture(0);

			// Return the singular mesh texture if subtextures are not found
            if (!tex) {
				tex = node->getMaterial(0).getTexture(0);
            }

			if (!tex)
			{
				return false;
			}

            texname = std::string(tex->getName().getPath().c_str());

            return true;
        }
    }

    return false;
}

std::string RenderManager::getMeshMaterialFromRay(vector3df start, vector3df end)
{
    line3df ray;
    ray.start = start;
    ray.end = end;

    vector3df point;
    triangle3df tri;

    ISceneNode* selectedNode = 0;
    selectedNode = m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, tri);

    if (!selectedNode) {
        return std::string();
    }

    std::string texname;
    if (getNodeTriangleTextureName(selectedNode, tri, texname)) {

#ifdef DEBUG_TEXTURE_PICKING_STATS
        auto windowWidth = 320, windowHeight = 300;
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
        ImGui::SetNextWindowPos(ImVec2(0, 175));
        if (ImGui::Begin("Texture Picking Debug", reinterpret_cast<bool*>(1),
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

            ImGui::Text("Texture  - %s", util::filenameFromPath(texname).c_str());
            ImGui::Text("Material - %s",
                _engine->material()->getMaterialName(
                    _engine->material()->getMaterialFromTexture(
                        util::filenameFromPath(texname).c_str())).c_str());
            ImGui::Text("SX - %f", ray.start.X);
            ImGui::Text("SY - %f", ray.start.Y);
            ImGui::Text("SZ - %f", ray.start.Z);
            ImGui::Text("EX - %f", ray.end.X);
            ImGui::Text("EY - %f", ray.end.Y);
            ImGui::Text("EZ - %f", ray.end.Z);

            ImGui::End();
        }
#endif
        return Utility::FilenameFromPath(texname);
    }

    return std::string();
}

std::string RenderManager::getMeshMaterialFromCameraRay(ICameraSceneNode *camera)
{
    ICameraSceneNode* cam = camera ? camera : m_sceneManager->getActiveCamera();

    camera->updateAbsolutePosition();

    line3df ray;
    ray.start = cam->getAbsolutePosition();
    ray.end = ray.start + (cam->getTarget() - ray.start).normalize() * 1000.0f;

    vector3df point;
    triangle3df tri;

    ISceneNode* selectedNode = 0;
    selectedNode = m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, tri);

    if (!selectedNode) {
        return std::string();
    }

    std::string texname;
    if (getNodeTriangleTextureName(selectedNode, tri, texname)) {

#ifdef DEBUG_TEXTURE_PICKING_STATS
        auto windowWidth = 320, windowHeight = 240;
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
        ImGui::SetNextWindowPos(ImVec2(0, 100));
        if (ImGui::Begin("Texture Picking Debug", reinterpret_cast<bool*>(1),
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

            ImGui::Text("Texture - %s", util::filenameFromPath(texname).c_str());
            ImGui::Text("SX - %f", ray.start.X);
            ImGui::Text("SY - %f", ray.start.Y);
            ImGui::Text("SZ - %f", ray.start.Z);
            ImGui::Text("EX - %f", ray.end.X);
            ImGui::Text("EY - %f", ray.end.Y);
            ImGui::Text("EZ - %f", ray.end.Z);

            ImGui::End();
        }
#endif	
        return Utility::FilenameFromPath(texname);
    }

    return std::string();
}

// Possible memory leak
void RenderManager::setNodeMesh(IAnimatedMesh* trimesh, IAnimatedMeshSceneNode* node, std::string file)
{
    auto meshptr = m_sceneManager->getMesh(file.c_str()); // <<< here


    if (!node && meshptr) {
        trimesh = meshptr;
        node = m_sceneManager->addAnimatedMeshSceneNode(trimesh, nullptr, 0xFFFF);
        return;
    }

    if (meshptr) {
        trimesh = meshptr; // <<< and here
        node->setMesh(meshptr);
        return;
    }

    spdlog::error("Failed to load mesh \'" + file + "\' in RenderManager::loadMesh");
}

RaycastResultData RenderManager::raycastWorldPosition(vector3df start, vector3df end, bool excludeDebugNodes)
{
    line3df ray;
    ray.start = start;
    ray.end = end;

    vector3df point;
    triangle3df tri;

    RaycastResultData data;

    auto node = m_sceneManager->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, point, tri, 0, nullptr, excludeDebugNodes);
    if (node) {

        data.hit = true;
        data.ray = ray;
        data.point = point;
        data.tri = tri;
        data.node = node;

        return data;
    }

    // DEPRECATED
    //log::write("Raycast operation failed in RenderManager::raycastWorldPosition - No valid node detected", LOG_WARNING);
    return RaycastResultData();
}

vector3df RenderManager::getBoneLocalPosition(IAnimatedMeshSceneNode *mesh, int bone)
{
    return mesh->getJointNode(bone)->getPosition();
}
vector3df RenderManager::getBoneWorldPosition(IAnimatedMeshSceneNode *mesh, int bone)
{
    return mesh->getJointNode(bone)->getAbsolutePosition();
}
vector3df RenderManager::getBoneLocalPosition(IAnimatedMeshSceneNode *mesh, std::string bone)
{
    return mesh->getJointNode(bone.c_str())->getPosition();
}
vector3df RenderManager::getBoneWorldPosition(IAnimatedMeshSceneNode *mesh, std::string bone)
{
    return mesh->getJointNode(bone.c_str())->getAbsolutePosition();
}

void RenderManager::renderText2D(
    irr::core::stringw text,
    TEXT_DEFAULT_FONT font,
    irr::core::rect<irr::s32> position,
    irr::video::SColor color,
    bool hcenter,
    bool vcenter) 
{
    switch (font) {
    case TEXT_DEFAULT_FONT::SMALL:
        m_textRenderableList.emplace_back(Text2D(g_DefaultTextRenderableFontSm, text, position, color, hcenter, vcenter));
        break;
    default:
        m_textRenderableList.emplace_back(Text2D(g_DefaultTextRenderableFontSm, text, position, color, hcenter, vcenter));
        break;
    }
}

void RenderManager::renderText2D(
	irr::core::stringw text,
	TEXT_DEFAULT_FONT font,
	irr::core::vector2di position,
	irr::video::SColor color,
	bool hcenter,
	bool vcenter)
{
	switch (font) {
	case TEXT_DEFAULT_FONT::SMALL:
		m_textRenderableList.emplace_back(Text2D(g_DefaultTextRenderableFontSm, text, irr::core::rect<irr::s32>(position.X, position.Y, 0, 0), color, hcenter, vcenter));
		break;
	default:
		m_textRenderableList.emplace_back(Text2D(g_DefaultTextRenderableFontSm, text, irr::core::rect<irr::s32>(position.X, position.Y, 0, 0), color, hcenter, vcenter));
		break;
	}
}


irr::core::vector3df RenderManager::GetInVector(irr::scene::ISceneNode* node)
{
    if (node)
    {
        irr::core::matrix4 mat = node->getRelativeTransformation();
        irr::core::vector3df in(mat[8], mat[9], mat[10]);
        in.normalize();
        return in;
    }

    return irr::core::vector3df(0, 0, 0);
}
irr::core::vector3df RenderManager::GetLeftVector(irr::scene::ISceneNode* node)
{
    if (node)
    {
        irr::core::matrix4 mat = node->getRelativeTransformation();
        irr::core::vector3df left(mat[0], mat[1], mat[2]);
        left.normalize();
        return left;
    }

    return irr::core::vector3df(0, 0, 0);
}
irr::core::vector3df RenderManager::GetUpVector(irr::scene::ISceneNode* node)
{
    if (node)
    {
        irr::core::matrix4 mat = node->getRelativeTransformation();
        irr::core::vector3df up(mat[4], mat[5], mat[6]);
        up.normalize();
        return up;
    }

    return irr::core::vector3df(0, 0, 0);
}

void RenderManager::saveConfiguration(RenderConfiguration& configuration)
{
	std::ofstream ofs_render("config/render.xml");
	cereal::XMLOutputArchive render_config(ofs_render);

	m_configuration = configuration;
	render_config(configuration);
}

void RenderManager::getResolutionFromMode(int aspect, int mode, int& width, int& height)
{
	switch (aspect)
	{
	case 0:
	{
		switch (mode)
		{
		case 0:
		{
			width = 640;
			height = 480;

			break;
		}
		case 1:
		{
			width = 800;
			height = 600;

			break;
		}
		case 2:
		{
			width = 1024;
			height = 768;

			break;
		}
		case 3:
		{
			width = 1280;
			height = 960;

			break;
		}
		case 4:
		{
			width = 1600;
			height = 1200;

			break;
		}
		case 5:
		{
			width = 2048;
			height = 1536;

			break;
		}
		default: break;
		}

		break;
	}
	case 1:
	{
		switch (mode)
		{
		case 0:
		{
			width = 1280;
			height = 1024;

			break;
		}
		case 1:
		{
			width = 2560;
			height = 2048;

			break;
		}
		default: break;
		}

		break;
	}
	case 2:
	{
		switch (mode)
		{
		case 0:
		{
			width = 1280;
			height = 720;

			break;
		}
		case 1:
		{
			width = 1366;
			height = 768;

			break;
		}
		case 2:
		{
			width = 1600;
			height = 900;

			break;
		}
		case 3:
		{
			width = 1920;
			height = 1080;

			break;
		}
		case 4:
		{
			width = 2550;
			height = 1440;

			break;
		}
		case 5:
		{
			width = 3840;
			height = 2160;

			break;
		}
		default: break;
		}

		break;
	}
	case 3:
	{
		switch (mode)
		{
		case 0:
		{
			width = 1280;
			height = 800;

			break;
		}
		case 1:
		{
			width = 1440;
			height = 900;

			break;
		}
		case 2:
		{
			width = 1680;
			height = 1050;

			break;
		}
		case 3:
		{
			width = 1920;
			height = 1200;

			break;
		}
		case 4:
		{
			width = 2560;
			height = 1600;

			break;
		}
		case 5:
		{
			width = 3840;
			height = 2400;

			break;
		}
		default: break;
		}

		break;
	}
	default: break;
	}
}
