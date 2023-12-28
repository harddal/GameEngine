#include "ILightManagerCustom.h"
#include "Engine/Input/InputMap.h"
#include "Engine/Input/InputManager.h"
#include "RenderManager.h"

irr::scene::ILightManagerCustom::ILightManagerCustom(irr::IrrlichtDevice* device, irr::video::SMaterials* mats)
    : Device(device),
      Materials(mats),
      TransparentRenderPass(false)
{
    irr::core::dimension2du dimension = Device->getVideoDriver()->getScreenSize();
    SolidBuffer = Device->getVideoDriver()->addRenderTargetTexture(dimension, "deferred-solid-buffer");

    FinalRender = 0;
    FinalRenderToTexture = false;

    //set up light mesh - sphere
    LightSphere = Device->getSceneManager()->addSphereSceneNode(1.0, 12);
    LightSphere->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
    LightSphere->setMaterialFlag(irr::video::EMF_FRONT_FACE_CULLING, true);
    LightSphere->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
    LightSphere->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    LightSphere->setVisible(false);

    //set up light mesh - cone
    /*LightCone= Device->getSceneManager()->addMeshSceneNode(Device->getSceneManager()->getGeometryCreator()->createConeMesh(1.0, 1.0, 8, irr::video::SColor(0,0,0,0), irr::video::SColor(0,0,0,0)));//Device->getSceneManager()->addSphereSceneNode(1.0, 12);
    LightCone->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
    LightCone->setMaterialFlag(irr::video::EMF_FRONT_FACE_CULLING, true);
    LightCone->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
    LightCone->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    LightCone->setAutomaticCulling(irr::scene::EAC_FRUSTUM_BOX);
    LightCone->setVisible(false);*/

    //set up light mesh - quad
    LightQuad = new irr::scene::IQuadSceneNode(0, Device->getSceneManager());
    LightQuad->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
    LightQuad->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    LightQuad->setVisible(false);
}

irr::scene::ILightManagerCustom::~ILightManagerCustom() 
{ 
	LightSphere->remove();
	LightQuad->remove();

	SolidBuffer->drop(); 
}

void irr::scene::ILightManagerCustom::OnPreRender(core::array<ISceneNode*>& lightList)
{
    // DEBUG: Tells renderer to render color, normal, and depth maps to this array of render targets
    Device->getVideoDriver()->setRenderTarget(MRTs, false, true);
}

void irr::scene::ILightManagerCustom::OnPostRender() {}

void irr::scene::ILightManagerCustom::OnRenderPassPreRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass)
{
    if (renderPass == irr::scene::ESNRP_TRANSPARENT) { TransparentRenderPass = true; }
}

int bitExtracted(int number, int k, int p)
{
    return (((1 << k) - 1) & (number >> p));
}

// Debug: Render the solid buffer as a lit scene to the screen
void irr::scene::ILightManagerCustom::OnRenderPassPostRender(irr::scene::E_SCENE_NODE_RENDER_PASS renderPass)
{
    if (renderPass == irr::scene::ESNRP_SOLID)
    {
        Device->getVideoDriver()->setRenderTarget(SolidBuffer, true, false);
        deferred();

        Device->getVideoDriver()->setRenderTarget(0, false, false);

        static int mode = -1;
#ifndef NDEBUG
        static bool f12 = false;
        if (InputManager::Get()->getKeyPressOnce(KEYBOARD_KEY::KEY_HOME, &f12))
        {
            mode++;

            if (mode > MRTs.size() - 1)
            {
                mode = -1;
            }
        }
#endif
        if (mode == -1)
        {
            // DEBUG: Draw the lit scene to the screen
            Device->getVideoDriver()->draw2DImage(SolidBuffer, irr::core::position2d<s32>(0, 0));
        }
#ifndef NDEBUG
        else if (mode >= 0)
        {
            if (mode == 0)
            {
                Device->getVideoDriver()->draw2DImage(MRTs[0].RenderTexture, irr::core::position2d<s32>(0, 0));
            }
            if (mode == 1)
            {
                Device->getVideoDriver()->draw2DImage(MRTs[1].RenderTexture, irr::core::position2d<s32>(0, 0));
            }
            if (mode == 2)
            {
                Device->getVideoDriver()->draw2DImage(MRTs[2].RenderTexture, irr::core::position2d<s32>(0, 0));
            }
        }
#endif
    }
    else if (renderPass == irr::scene::ESNRP_TRANSPARENT)
    {
        TransparentRenderPass = false;
    }
}

void irr::scene::ILightManagerCustom::OnNodePreRender(irr::scene::ISceneNode* node)
{
    if (TransparentRenderPass) {
        if (node->getMaterial(0).MaterialType == Materials->Transparent) {
            node->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
            node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        }
        else if (node->getMaterial(0).MaterialType == Materials->TransparentSoft) {
            node->setMaterialTexture(1, MRTs[2].RenderTexture);
            node->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, false);
        }
    }
}

void irr::scene::ILightManagerCustom::OnNodePostRender(irr::scene::ISceneNode* node) {}

inline void irr::scene::ILightManagerCustom::deferred()
{
    //render ambient
    LightQuad->setMaterialType(LightAmbientMaterial);
    for (irr::u32 i = 0; i < MRTs.size(); i++) { LightQuad->setMaterialTexture(i, MRTs[i].RenderTexture); }
    LightQuad->render(); //also renders nodes with no lighting

    //render dynamic lights
    for (irr::u32 i = 0; i < Device->getVideoDriver()->getDynamicLightCount(); i++) {
        irr::video::SLight light = Device->getVideoDriver()->getDynamicLight(i);

        //point
        if (light.Type == irr::video::ELT_POINT) {
            LightSphere->setMaterialType(LightPointMaterial);
            LightPointCallback->updateConstants(light);
            LightSphere->setScale(irr::core::vector3df(light.Radius * 2.0f));
            LightSphere->setPosition(light.Position);
            LightSphere->updateAbsolutePosition();
            LightSphere->render();
        }

            //spot //using sphere instead of a cone as a hack
        else if (light.Type == irr::video::ELT_SPOT) {
            /*LightSpotCallback->updateConstants(light);
                       LightCone->setScale(irr::core::vector3df(LightSpotCallback->getConeRadius(), 
                            light.Radius*1.4, LightSpotCallback->getConeRadius()));
                       // Need to do some more calculations because the cone mesh is kinda fucked
                       LightCone->setRotation(light.Direction.getHorizontalAngle() + 
                            irr::core::vector3df(-90.0, 0.0, 0.0));
                       LightCone->setPosition(light.Position + light.Direction*light.Radius);
                       LightCone->updateAbsolutePosition();
                       LightCone->render();*/
			
            LightSphere->setMaterialType(LightSpotMaterial);
            LightSpotCallback->updateConstants(light);
            LightSphere->setScale(irr::core::vector3df(light.Radius));
            LightSphere->setPosition(light.Position + light.Direction * light.Radius);
			LightSphere->updateAbsolutePosition();
            LightSphere->render();
        }

            //directional
        else if (light.Type == irr::video::ELT_DIRECTIONAL) {
            LightDirectionalCallback->updateConstants(light);
            LightQuad->setMaterialType(LightDirectionalMaterial);
            LightQuad->render();
        }
    }
}


void irr::scene::ILightManagerCustom::setMRTs(irr::core::array<irr::video::IRenderTarget>& mrts)
{
    MRTs = mrts;

    for (irr::u32 i = 0; i < MRTs.size(); i++) {
        LightSphere->setMaterialTexture(i, MRTs[i].RenderTexture);
        //LightCone->setMaterialTexture(i, MRTs[i].RenderTexture);
        LightQuad->setMaterialTexture(i, MRTs[i].RenderTexture);
    }
}

void irr::scene::ILightManagerCustom::clearMRTs()
{
	for (irr::u32 i = 0; i < MRTs.size(); i++) {
		LightSphere->setMaterialTexture(i, 0);
		//LightCone->setMaterialTexture(i, MRTs[i].RenderTexture);
		LightQuad->setMaterialTexture(i, 0);
	}
}

void irr::scene::ILightManagerCustom::setRenderTexture(irr::video::ITexture* tex) { FinalRender = tex; }

void irr::scene::ILightManagerCustom::setDoFinalRenderIntoTexture(bool well) { FinalRenderToTexture = well; }

bool irr::scene::ILightManagerCustom::getDoFinalRenderToTexture() const { return FinalRenderToTexture; }

irr::video::ITexture* irr::scene::ILightManagerCustom::getRenderTexture()
{
    if (FinalRender) return FinalRender;
    else return 0;
}


void irr::scene::ILightManagerCustom::setLightPointMaterialType(irr::video::E_MATERIAL_TYPE& type)
{
    LightPointMaterial = type;
}

void irr::scene::ILightManagerCustom::setLightPointCallback(irr::video::IShaderPointLightCallback* callback)
{
    LightPointCallback = callback;
}

void irr::scene::ILightManagerCustom::setLightSpotMaterialType(irr::video::E_MATERIAL_TYPE& type)
{
    LightSpotMaterial = type;
}

void irr::scene::ILightManagerCustom::setLightSpotCallback(irr::video::IShaderSpotLightCallback* callback)
{
    LightSpotCallback = callback;
}

void irr::scene::ILightManagerCustom::setLightDirectionalMaterialType(irr::video::E_MATERIAL_TYPE& type)
{
    LightDirectionalMaterial = type;
}

void irr::scene::ILightManagerCustom::setLightDirectionalCallback(irr::video::IShaderDirectionalLightCallback* callback)
{
    LightDirectionalCallback = callback;
}

void irr::scene::ILightManagerCustom::setLightAmbientMaterialType(irr::video::E_MATERIAL_TYPE& type)
{
    LightAmbientMaterial = type;
}

void irr::scene::ILightManagerCustom::setLightAmbientCallback(irr::video::IShaderAmbientLightCallback* callback)
{
    LightAmbientCallback = callback;
}


bool irr::scene::ILightManagerCustom::isAABBinFrustum(irr::core::aabbox3d<irr::f32> box,
                                                      const irr::scene::SViewFrustum* frustum) const
{
    for (irr::u32 i = 0; i < irr::scene::SViewFrustum::VF_PLANE_COUNT; i++) {
        if (box.classifyPlaneRelation(frustum->planes[i]) == irr::core::ISREL3D_BACK) return true;
    }
    return false;
}
