#include "CShaderLibrary.h"

#include <spdlog/spdlog.h>
#include "Utility/Utility.h"

irr::video::CShaderLibrary::CShaderLibrary(const irr::c8* shaderDir, irr::IrrlichtDevice* dev)
{
    ShaderDir = shaderDir;
    Device = dev;

    Shaders.push_back(SShaderSource());
}

// TODO: Crashes program if shader does not exist
void irr::video::CShaderLibrary::loadShader(const irr::c8* name, const irr::c8* sourceVertex,
                                            const irr::c8* sourcePixel)
{
    SShaderSource newShader;
    newShader.Name = name;

    irr::core::stringc fileNameVertex;
    fileNameVertex.append(ShaderDir);
    fileNameVertex.append(sourceVertex);
    irr::io::IReadFile* fileVertex = Device->getFileSystem()->createAndOpenFile(fileNameVertex);
    if (!fileVertex) {
        spdlog::error("Failed to load vertex shader program: {}", sourceVertex);
        Utility::Error("Failed to load vertex shader program: " + std::string(sourceVertex));
        return;
    }
    irr::u32 size = fileVertex->getSize();
    irr::c8* buff = new irr::c8 [size];
    fileVertex->read(buff, fileVertex->getSize());
    newShader.SourceVertex = buff;
    newShader.SourceVertex[fileVertex->getSize()] = '\0';

    irr::core::stringc fileNamePixel;
    fileNamePixel.append(ShaderDir);
    fileNamePixel.append(sourcePixel);
    io::IReadFile* filePixel = Device->getFileSystem()->createAndOpenFile(fileNamePixel);
    if (!filePixel) {
        spdlog::error("Failed to load fragment shader program: {}", sourcePixel);
        Utility::Error("Failed to load fragment shader program: " + std::string(sourcePixel));
        return;
    }
    size = filePixel->getSize();
    irr::c8* buff2 = new irr::c8 [size];
    filePixel->read(buff2, filePixel->getSize());
    newShader.SourcePixel = buff2;
    newShader.SourcePixel[filePixel->getSize()] = '\0';

    Shaders.push_back(newShader);
}

irr::video::SShaderSource& irr::video::CShaderLibrary::getShader(const irr::c8* name)
{
    for (irr::u16 i = 0; i < Shaders.size(); i++) { if (Shaders[i].Name == name) return Shaders[i]; }
    return Shaders[0];
}
