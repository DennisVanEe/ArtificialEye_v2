#include "HalfUniColorTextPack.hpp"

ee::HalfUniColorTextPack::HalfUniColorTextPack(glm::vec4 color) :
    m_color(color)
{
}

void ee::HalfUniColorTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    shader->assignColor("u_modelColor", m_color);
}

std::string ee::HalfUniColorTextPack::getVShaderName() const
{
    return "modelHalfUniColor_vert";
}

std::string ee::HalfUniColorTextPack::getFShaderName() const
{
    return "modelHalfUniColor_frag";
}

