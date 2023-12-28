#ifndef CRENDERER_H
#define CRENDERER_H

#include <irrlicht.h>

#include "ILightManagerCustom.h"
#include "SMaterials.h"
#include "CShaderLibrary.h"
#include "MaterialCallbacks.h"
#include "CMaterialSwapper.h"
#include "IShaderDefaultCallback.h"


namespace irr
{
    namespace video
    {
        class CRenderer
        {
        public:
            CRenderer(irr::IrrlichtDevice* device, const irr::c8* shaderDir);
            ~CRenderer();

            void createDefaultPipeline();

			static irr::video::CRenderer* createRenderer(irr::IrrlichtDevice* device, const irr::c8* shaderDir = "content/shader/");

			void resetMRTs();

            void clearMRTs();

    
            void createMRT(const irr::c8* name, irr::video::ECOLOR_FORMAT format = irr::video::ECF_A8R8G8B8,
                           irr::core::dimension2du dimension = irr::core::dimension2du(0, 0));

            irr::video::ITexture* getMRT(irr::u32 index);

       
            irr::u32 getMRTCount() const;

   
            void setDoFinalRenderToTexture(bool shouldI);

            irr::video::ITexture* getFinalRenderTexture() const;


            irr::s32 createMaterial(irr::video::SShaderSource shader,
                                    irr::video::IShaderConstantSetCallBack* callback = 0,
                                    irr::video::E_MATERIAL_TYPE baseType = irr::video::EMT_SOLID);


            irr::video::CShaderLibrary* getShaderLibrary() const;


            irr::video::SMaterials* getMaterials() const;

            irr::video::CMaterialSwapper* getMaterialSwapper() const;
            irr::IrrlichtDevice* getDevice() const;


        private:
            void loadShaders();

			irr::video::IShaderPointLightCallback* pointCallback;
			irr::video::IShaderSpotLightCallback* spotCallback;
			irr::video::IShaderDirectionalLightCallback* directionalCallback;
			irr::video::IShaderAmbientLightCallback* ambientCallback;

            irr::IrrlichtDevice* Device;
            irr::scene::ILightManagerCustom* LightMgr;
            irr::video::CShaderLibrary* ShaderLib;
            irr::video::SMaterials* Materials;
            irr::video::CMaterialSwapper* MaterialSwapper;

			irr::video::ITexture *deferred_mrt_color, *deferred_mrt_normal, *deferred_mrt_depth;

            irr::core::array<irr::video::IRenderTarget> MRTs;
        };
    }
}

#endif
