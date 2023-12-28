#pragma once

#include "irrlicht.h"

#include "Engine/Renderer/CRenderer.h"
#include "cereal/cereal.hpp"
#include "IrrIMGUI.h"
#include "XEffects.h"

#define DPI_SCALED_IMVEC2(x, y) ImVec2(x * RenderManager::Get()->getConfiguration().dpi_scale, y * RenderManager::Get()->getConfiguration().dpi_scale)

struct RenderConfiguration
{
    bool
        fullscreen,
        vSync;

    int
        api,
        width,
        height,
		editor_width,
		editor_height,
        depth,
		aspect,
		mode,
		dpi_scale,
        anisotropyFactor,
        antialiasingFactor,
        frameLimit;

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(fullscreen),
            CEREAL_NVP(vSync),
            CEREAL_NVP(api),
            CEREAL_NVP(width),
            CEREAL_NVP(height),
			CEREAL_NVP(editor_width),
			CEREAL_NVP(editor_height),
            CEREAL_NVP(depth),
			CEREAL_NVP(aspect),
			CEREAL_NVP(mode),
			CEREAL_NVP(dpi_scale),
            CEREAL_NVP(anisotropyFactor),
            CEREAL_NVP(antialiasingFactor),
            CEREAL_NVP(frameLimit));
    }

    RenderConfiguration() :
        fullscreen(false),
        vSync(false),
        api(0),
        width(1024),
        height(768),
		editor_width(1024),
		editor_height(768),
        depth(32),
		aspect(0),
		mode(0),
		dpi_scale(1),
        anisotropyFactor(0),
        antialiasingFactor(0),
        frameLimit(60)
    {}
};

enum class TEXT_DEFAULT_FONT
{
    // 16x10
    SMALL
};

// 16x10
extern irr::gui::IGUIFont* g_DefaultTextRenderableFontSm;

struct Text2D
{
    Text2D() : hcenter(false), vcenter(false) {}
    Text2D(
        irr::gui::IGUIFont* font,
        irr::core::stringw text,
        irr::core::rect<irr::s32> position = irr::core::rect<irr::s32>(0, 0, 0, 0),
        irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
        bool hcenter = false,
        bool vcenter = false)
    {
        this->font = font;
        this->text = text;
        this->position = position;
        this->color = color;
        this->hcenter = hcenter;
        this->vcenter = vcenter;
    }

    bool hcenter, vcenter;

    irr::gui::IGUIFont* font;
    irr::core::stringw text;
    irr::core::rect<irr::s32> position;
    irr::video::SColor color;
};

struct Line3D
{
    irr::core::line3df line;
    irr::video::SColor color;

    Line3D() {}
    Line3D(irr::core::line3df line, irr::video::SColor color = irr::video::SColor(255, 255, 255, 255))
    {
        this->line = line;
        color.setAlpha(255);
        this->color = color;
    }
};

struct Image2D
{
    Image2D(
        irr::video::ITexture* image,
        irr::core::vector2di position = irr::core::vector2di(0, 0),
        irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
        bool use_alpha = true,
        bool use_center = false)
    {
        this->image = image;
        this->use_alpha = use_alpha;
        this->position = position;
        this->color = color;
        this->use_center = use_center;
    }

    bool use_alpha;
    // NOIMP
    bool use_center;

    irr::core::vector2di position;

    irr::video::ITexture* image;

    irr::video::SColor color;
};

struct RaycastResultData
{
    bool hit;

    irr::core::line3df ray;
    irr::core::vector3df point;
    irr::core::triangle3df tri;

    irr::scene::ISceneNode *node;

    RaycastResultData() :
        hit(false), node(nullptr) {}
};

struct ShaderMaterial
{
    std::string name;
    irr::s32 material;

    ShaderMaterial(std::string name, irr::s32 material = 0)
    {
        this->name = name;
        this->material = material;
    }
};

class ShaderMaterialManager
{
public:
    static void add(ShaderMaterial material);
    static irr::video::E_MATERIAL_TYPE get(std::string name);

private:
    static std::vector<ShaderMaterial> s_ShaderMaterialList;
};

class ShaderConstantSetCallBack : public irr::video::IShaderConstantSetCallBack
{
public:
    void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;
};

class RenderManager
{
public:
    RenderManager& operator =(const RenderManager&) = delete;

    RenderManager(const std::string& name = std::string(), const std::string& args = std::string());
    ~RenderManager();

	void getResolutionFromMode(int aspect, int mode, int& width, int& height);
	
    void beginImGui();

    void draw(irr::f32 dt);

    void onWindowResizeEvent();

    void createShaderMaterial(
        std::string name, std::string fragment, std::string vertex,
        irr::video::E_MATERIAL_TYPE materialType = irr::video::EMT_SOLID,
        irr::video::E_PIXEL_SHADER_TYPE psVersion = irr::video::EPST_PS_2_0,
        irr::video::E_VERTEX_SHADER_TYPE vsVersion = irr::video::EVST_VS_2_0);

    void createDefaultShaders();

    irr::IrrlichtDevice*                 device()       const { return m_device; }
    irr::video::IVideoDriver*            driver()       const { return m_driver; }
    irr::scene::ISceneManager*           sceneManager() const { return m_sceneManager; }
    irr::video::IGPUProgrammingServices* gpu()          const { return m_gpu; }
    irr::gui::IGUIEnvironment*           gui()          const { return m_gui; }
    irr::video::CRenderer*               renderer()     const { return m_renderer; }
	IrrIMGUI::IIMGUIHandle*              imgui()        const { return m_imgui; }
	EffectHandler*                       effect()       const { return m_effectManager; }

    void setAmbientLight(irr::video::SColor color = irr::video::SColor(255, 255, 255, 255)) const { m_sceneManager->setAmbientLight(color); }

    void initDefaultSkyDome(std::string texture = "content/texture/color/black.png");
    void removeDefaultSkyDome();
    void swapSkyDomeTexture(std::string texture = "content/texture/color/black.png");
    std::string getCurrentSkydomeTexture() const { return m_currentSkydomeTexture; }

    void swapSceneNodeMaterialAll() const { m_renderer->getMaterialSwapper()->swapMaterials(); }

    irr::scene::ISceneNode *getNodeFromCursorPosition(irr::scene::ICameraSceneNode *camera = nullptr);
    irr::core::vector3df getPoint3DFromCursorPosition(irr::scene::ICameraSceneNode *camera = nullptr);

    irr::scene::ISceneNode* getNodeFromRaycast(irr::core::vector3df start, irr::core::vector3df end);

    float getRaycastLength(irr::core::vector3df startpoint, RaycastResultData& data) const { return hypot(hypot(startpoint.X - data.point.X, startpoint.Y - data.point.Y), startpoint.Z - data.point.Z); }

    bool getNodeTriangleTextureName(irr::scene::ISceneNode* node, const irr::core::triangle3df& tri, std::string& texname);
    std::string getMeshMaterialFromRay(irr::core::vector3df start, irr::core::vector3df end);
    std::string getMeshMaterialFromCameraRay(irr::scene::ICameraSceneNode *camera = nullptr);

    RaycastResultData raycastWorldPosition(irr::core::vector3df start, irr::core::vector3df end, bool excludeDebugNodes = false);

	static bool getAABBIntersection(irr::scene::ISceneNode* n1, irr::scene::ISceneNode* n2) { return n1->getTransformedBoundingBox().intersectsWithBox(n2->getTransformedBoundingBox()); }
	static bool calculate_bbox_collision(irr::scene::IAnimatedMeshSceneNode* sn1, irr::scene::IAnimatedMeshSceneNode* sn2) { return sn1->getTransformedBoundingBox().intersectsWithBox(sn2->getTransformedBoundingBox()); }

    void setNodeMesh(irr::scene::IAnimatedMesh* trimesh, irr::scene::IAnimatedMeshSceneNode* node, std::string file);

    irr::core::vector3df getBoneLocalPosition(irr::scene::IAnimatedMeshSceneNode *mesh, int bone);
    irr::core::vector3df getBoneWorldPosition(irr::scene::IAnimatedMeshSceneNode *mesh, int bone);
    irr::core::vector3df getBoneLocalPosition(irr::scene::IAnimatedMeshSceneNode *mesh, std::string bone);
    irr::core::vector3df getBoneWorldPosition(irr::scene::IAnimatedMeshSceneNode *mesh, std::string bone);

    void renderLine3D(const Line3D& line) { m_lineRenderableList.emplace_back(line); }

    void renderText2D(
        irr::core::stringw text,
        irr::gui::IGUIFont* font = g_DefaultTextRenderableFontSm,
        irr::core::rect<irr::s32> position = irr::core::rect<irr::s32>(0, 0, 0, 0),
        irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
        bool hcenter = false,
        bool vcenter = false) { m_textRenderableList.emplace_back(Text2D(font, text, position, color, hcenter, vcenter)); }
    void renderText2D(
        irr::core::stringw text,
        TEXT_DEFAULT_FONT font,
        irr::core::rect<irr::s32> position = irr::core::rect<irr::s32>(0, 0, 0, 0),
        irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
        bool hcenter = false,
        bool vcenter = false);

	void renderText2D(
		irr::core::stringw text,
		irr::gui::IGUIFont* font = g_DefaultTextRenderableFontSm,
		irr::core::vector2di position = irr::core::vector2di(0, 0),
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		bool hcenter = false,
		bool vcenter = false) {
		m_textRenderableList.emplace_back(Text2D(font, text, irr::core::rect<irr::s32>(position.X, position.Y, 0, 0), color, hcenter, vcenter));
	}
	void renderText2D(
		irr::core::stringw text,
		TEXT_DEFAULT_FONT font,
		irr::core::vector2di position = irr::core::vector2di(0, 0),
		irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
		bool hcenter = false,
		bool vcenter = false);

    void renderImage2D(
        irr::video::ITexture* image,
        irr::core::vector2di position = irr::core::vector2di(0, 0),
        irr::video::SColor color = irr::video::SColor(255, 255, 255, 255),
        bool use_alpha = true,
        bool use_center = false) { m_imageRenderableList.emplace_back(Image2D(image, position, color, use_alpha, use_center)); }

    static irr::core::vector3df GetInVector(irr::scene::ISceneNode* node);
    static irr::core::vector3df GetLeftVector(irr::scene::ISceneNode* node);
    static irr::core::vector3df GetUpVector(irr::scene::ISceneNode* node);

    RenderConfiguration getConfiguration() const { return m_configuration; }
	void saveConfiguration(RenderConfiguration& configuration);

    static RenderManager* Get() { return s_Instance; }

private:
    static RenderManager* s_Instance;

    bool m_drawDebugStatistics;

    irr::scene::ICameraSceneNode* m_defaultCamera;

    RenderConfiguration m_configuration;

    irr::SIrrlichtCreationParameters m_irrlichtParams;

    irr::IrrlichtDevice* m_device;
    irr::gui::IGUIEnvironment* m_gui;
    irr::video::IVideoDriver* m_driver;
    irr::video::IGPUProgrammingServices* m_gpu;
    irr::scene::ISceneManager* m_sceneManager;
    irr::video::CRenderer* m_renderer;
	EffectHandler* m_effectManager;

    ShaderConstantSetCallBack* m_shaderConstantCallBack;

    IrrIMGUI::CIMGUIEventReceiver m_imguiEventReceiver;
    IrrIMGUI::IIMGUIHandle* m_imgui;

    irr::video::SColor m_backgroundColor;

    std::string m_currentSkydomeTexture;
    irr::scene::ISceneNode* m_defaultSkyDome;

    std::vector<Text2D> m_textRenderableList;
    std::vector<Image2D> m_imageRenderableList;
    std::vector<Line3D> m_lineRenderableList;

	irr::video::ITexture* m_effectRenderTarget;
};
