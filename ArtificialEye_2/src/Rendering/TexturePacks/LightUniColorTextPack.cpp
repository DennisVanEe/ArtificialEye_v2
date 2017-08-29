#include "LightUniColorTextPack.hpp"

ee::LightUniColorTextPack::LightUniColorTextPack(Vector3 lightPosition, Vector3 color, ShaderMaterial material) :
    TexturePack(),
    m_lightPosition(lightPosition),
    m_color(color),
    m_material(material)
{
}

ee::LightUniColorTextPack::LightUniColorTextPack() :
    TexturePack(),
    m_color(F(1), F(1), F(1))
{
}

void ee::LightUniColorTextPack::setTexture(Shader* shader, const ShaderMaterial* material, const Camera* camera)
{
    shader->assignColor("u_modelColor", m_color);
    shader->assignVec3f("u_modelMaterial.m_ambient", m_material.m_ambient);
    shader->assignVec3f("u_modelMaterial.m_diffuse", m_material.m_diffuse);
    shader->assignVec3f("u_lightPosition", m_lightPosition);
}

ee::TexturePack* ee::LightUniColorTextPack::getCopy() const
{
    return new LightUniColorTextPack(*this);
}

std::string ee::LightUniColorTextPack::getVShaderName() const
{
    return "modelLightUniColor_vert";
}

std::string ee::LightUniColorTextPack::getFShaderName() const
{
    return "modelLightUniColor_frag";
}