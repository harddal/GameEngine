#ifndef CSHADERS_H
#define CSHADERS_H

#include <irrlicht.h>

namespace irr
{
    namespace video
    {
        struct SShaderSource
        {
            irr::core::stringc Name;
            irr::core::stringc SourceVertex;
            irr::core::stringc SourcePixel;
        };

        // Use irr::video::CRenderer::getShaderLibrary() to instance this class
        class CShaderLibrary
        {
        public:
            CShaderLibrary(const irr::c8* shaderDir, irr::IrrlichtDevice* dev);

            void loadShader(const irr::c8* name, const irr::c8* sourceVertex, const irr::c8* sourcePixel);

            SShaderSource& getShader(const irr::c8* name);

        private:
            irr::core::stringc ShaderDir;
            irr::IrrlichtDevice* Device;
            irr::core::array<SShaderSource> Shaders;
            
        };
    }
}

#endif
