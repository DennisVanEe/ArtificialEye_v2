#include "SkyBoxTextPack.hpp"

ee::SkyBoxTextPack::SkyBoxTextPack(std::string rootDir, const std::vector<std::string>& faceNames) : 
    m_skyBox(loadCubeMap(rootDir, faceNames)) 
{
}

ee::SkyBoxTextPack::SkyBoxTextPack(SkyBoxTextPack && other) : 
    m_skyBox(std::move(other.m_skyBox)) 
{
}

void ee::SkyBoxTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{ 
    shader->bindTexture(GL_TEXTURE_CUBE_MAP, 0, m_skyBox.getTexture()); 
}

std::string ee::SkyBoxTextPack::getVShaderName() const 
{ 
    return "skyBox_vert"; 
}

std::string ee::SkyBoxTextPack::getFShaderName() const 
{ 
    return "skyBox_frag";
}
