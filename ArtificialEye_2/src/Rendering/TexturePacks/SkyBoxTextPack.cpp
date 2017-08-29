#include "SkyBoxTextPack.hpp"

ee::SkyBoxTextPack::SkyBoxTextPack(CubeMap skyBox) :
    m_skyBox(std::move(skyBox))
{
}

void ee::SkyBoxTextPack::setTexture(Shader* shader, const ShaderMaterial * material, const Camera * camera)
{
    shader->bindTexture(GL_TEXTURE_CUBE_MAP, 0, m_skyBox.getTexture());
}

ee::TexturePack* ee::SkyBoxTextPack::getCopy() const
{
    return new SkyBoxTextPack(*this);
}

std::string ee::SkyBoxTextPack::getVShaderName() const
{
    return "skyBox_vert";
}

std::string ee::SkyBoxTextPack::getFShaderName() const
{
    return "skyBox_frag";
}