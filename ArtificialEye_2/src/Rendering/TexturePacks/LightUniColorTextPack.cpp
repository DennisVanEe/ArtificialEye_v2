#include "LightUniColorTextPack.hpp"

ee::LightUniColorTextPack::LightUniColorTextPack(glm::vec3 lightPosition, glm::vec3 color, ShaderMaterial material) :
    TexturePack(),
    m_lightPosition(lightPosition),
    m_color(color),
    m_material(material)
{
}

ee::LightUniColorTextPack::LightUniColorTextPack() :
    TexturePack(),
    m_color(1.f, 1.f, 1.f)
{
}

void ee::LightUniColorTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    shader->assignColor("u_modelColor", m_color);
    shader->assignVec3("u_modelMaterial.m_ambient", m_material.m_ambient);
    shader->assignVec3("u_modelMaterial.m_diffuse", m_material.m_diffuse);
    shader->assignVec3("u_lightPosition", m_lightPosition);
}

std::string ee::LightUniColorTextPack::getVShaderName() const
{
    return "modelLightUniColor_vert";
}

std::string ee::LightUniColorTextPack::getFShaderName() const
{
    return "modelLightUniColor_frag";
}