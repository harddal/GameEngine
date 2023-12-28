#ifndef MATERIALCALLBACKS_H_INCLUDED
#define MATERIALCALLBACKS_H_INCLUDED

#include <irrlicht.h>

namespace irr
{
    namespace video
    {
        class IShaderPointLightCallback : public irr::video::IShaderConstantSetCallBack
        {
        public:
            IShaderPointLightCallback(irr::scene::ISceneManager* smgr) { Smgr = smgr; }

            virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
            {
                irr::s32 tex0 = 0;
                irr::s32 tex1 = 1;
                irr::s32 tex2 = 2;
                services->setPixelShaderConstant("ColorTex", &tex0, 1);
                services->setPixelShaderConstant("NormalTex", &tex1, 1);
                services->setPixelShaderConstant("DepthTex", &tex2, 1);

                irr::scene::ICameraSceneNode* cam = Smgr->getActiveCamera();
                irr::core::matrix4 worldMat = services->getVideoDriver()->getTransform(irr::video::ETS_WORLD);
                irr::core::matrix4 viewMat = services->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
                irr::core::matrix4 projMat = services->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
                irr::core::matrix4 worldViewProj = projMat * viewMat * worldMat; //worldMat * viewMat * projMat;

                services->setVertexShaderConstant("WorldViewProjMat", worldViewProj.pointer(), 16);

                irr::f32 farDist;
                if (projMat[10] > 0.f) farDist = -projMat[14] / (projMat[10] - 1.f); // Left Handed
                else farDist = projMat[14] / (projMat[10] + 1.f); // Right Handed
                services->setPixelShaderConstant("CamFar", &farDist, 1);

                viewMat.transformVect(Pos);
                services->setPixelShaderConstant("Position", (irr::f32*)&Pos, 3);
                services->setPixelShaderConstant("Radius", &Radius, 1);
                services->setPixelShaderConstant("Color", (irr::f32*)&Color, 3);

                irr::core::vector3df farLeftUp = cam->getViewFrustum()->getFarLeftUp();
                viewMat.transformVect(farLeftUp);
                services->setVertexShaderConstant("VertexFarLeftUp", (irr::f32*)&farLeftUp, 3);
            }

            virtual void updateConstants(irr::video::SLight& light)
            {
                Pos = light.Position;
                Radius = light.Radius * 1.5f;
                Color = light.DiffuseColor;
            }

        private:
            irr::scene::ISceneManager* Smgr;
            irr::core::vector3df Pos;
            irr::video::SColorf Color;
            irr::f32 Radius;
        };


        class IShaderSpotLightCallback : public irr::video::IShaderConstantSetCallBack
        {
        public:
            IShaderSpotLightCallback(irr::scene::ISceneManager* smgr) { Smgr = smgr; }

            virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
            {
                irr::s32 tex0 = 0;
                irr::s32 tex1 = 1;
                irr::s32 tex2 = 2;
                services->setPixelShaderConstant("ColorTex", &tex0, 1);
                services->setPixelShaderConstant("NormalTex", &tex1, 1);
                services->setPixelShaderConstant("DepthTex", &tex2, 1);

                irr::scene::ICameraSceneNode* cam = Smgr->getActiveCamera();
                irr::core::matrix4 worldMat = services->getVideoDriver()->getTransform(irr::video::ETS_WORLD);
                irr::core::matrix4 viewMat = services->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
                irr::core::matrix4 projMat = services->getVideoDriver()->getTransform(irr::video::ETS_PROJECTION);
                irr::core::matrix4 worldViewProj = projMat * viewMat * worldMat; //worldMat * viewMat * projMat;

                services->setVertexShaderConstant("WorldViewProjMat", worldViewProj.pointer(), 16);

                irr::f32 farDist;
                if (projMat[10] > 0.f) farDist = -projMat[14] / (projMat[10] - 1.f); // Left Handed
                else farDist = projMat[14] / (projMat[10] + 1.f); // Right Handed
                services->setPixelShaderConstant("CamFar", &farDist, 1);

                viewMat.transformVect(Pos);
                viewMat.rotateVect(Direction);
                services->setPixelShaderConstant("Position", (irr::f32*)&Pos, 3);
                services->setPixelShaderConstant("Radius", &Radius, 1);
                services->setPixelShaderConstant("Color", (irr::f32*)&Color, 3);
                services->setPixelShaderConstant("Direction", (irr::f32*)&Direction, 3);
                services->setPixelShaderConstant("CosCutoff", (irr::f32*)&CosCutoff, 1);
                services->setPixelShaderConstant("Falloff", (irr::f32*)&Falloff, 1);

                irr::core::vector3df farLeftUp = cam->getViewFrustum()->getFarLeftUp();
                viewMat.transformVect(farLeftUp);
                services->setVertexShaderConstant("VertexFarLeftUp", (irr::f32*)&farLeftUp, 3);
            }

            virtual void updateConstants(irr::video::SLight& light)
            {
                Pos = light.Position;
                Radius = light.Radius * 1.5f;
                Color = light.DiffuseColor;
                Direction = light.Direction;
                CosCutoff = static_cast<irr::f32>(cos((light.OuterCone / 2.0f) * irr::core::DEGTORAD));
                Falloff = light.Falloff;      
                ConeRadius = static_cast<irr::f32>(((Radius / (sin((180.0f - light.OuterCone) / 2.0f))) * sin(light.OuterCone)) / 2.0f);
            }

            virtual irr::f32 getConeRadius() { return ConeRadius; }

        private:
            irr::scene::ISceneManager* Smgr;
            irr::core::vector3df Pos;
            irr::video::SColorf Color;
            irr::core::vector3df Direction;
            irr::f32 Radius;
            irr::f32 CosCutoff;
            irr::f32 ConeRadius;
            irr::f32 Falloff;
        };


        class IShaderDirectionalLightCallback : public irr::video::IShaderConstantSetCallBack
        {
        public:
            IShaderDirectionalLightCallback(irr::scene::ISceneManager* smgr) { Smgr = smgr; }

            virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
            {
                irr::s32 tex0 = 0;
                irr::s32 tex1 = 1;
                irr::s32 tex2 = 2;
                services->setPixelShaderConstant("ColorTex", &tex0, 1);
                services->setPixelShaderConstant("NormalTex", &tex1, 1);
                services->setPixelShaderConstant("DepthTex", &tex2, 1);

                irr::core::matrix4 viewMat = Smgr->getVideoDriver()->getTransform(irr::video::ETS_VIEW);
                viewMat.rotateVect(Direction);
                services->setPixelShaderConstant("Direction", (irr::f32*)&Direction, 3);

                services->setPixelShaderConstant("Color", (irr::f32*)&Color, 3);
            }

            virtual void updateConstants(irr::video::SLight& light)
            {
                Direction = light.Direction;
                Color = light.DiffuseColor;
            }

        private:
            irr::scene::ISceneManager* Smgr;
            irr::core::vector3df Direction;
            irr::video::SColorf Color;
        };


        class IShaderAmbientLightCallback : public irr::video::IShaderConstantSetCallBack
        {
        public:
            IShaderAmbientLightCallback(irr::scene::ISceneManager* smgr) { Smgr = smgr; }

            virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
            {
                irr::s32 tex0 = 0;
                irr::s32 tex1 = 1;
                services->setPixelShaderConstant("ColorTex", &tex0, 1);
                services->setPixelShaderConstant("NormalTex", &tex1, 1);

                irr::core::vector3df ambientCol;
                ambientCol.X = Smgr->getAmbientLight().r;
                ambientCol.Y = Smgr->getAmbientLight().g;
                ambientCol.Z = Smgr->getAmbientLight().b;
                services->setPixelShaderConstant("Color", (irr::f32*)&ambientCol, 3);
            }

        private:
            irr::scene::ISceneManager* Smgr;
        };
    }
}


#endif
