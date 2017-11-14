#include "LineUniColorTextPack.hpp"

ee::LineUniColorTextPack::LineUniColorTextPack(glm::vec3 color) : 
    m_color(color) 
{
}

ee::LineUniColorTextPack::LineUniColorTextPack() : 
    m_color(1.f, 1.f, 1.f) 
{
}

void ee::LineUniColorTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    shader->assignColor("u_color", m_color);
}

void ee::LineUniColorTextPack::postDraw() 
{
}

std::string ee::LineUniColorTextPack::getVShaderName() const 
{ 
    return "lineUniColor_vert"; 
}

std::string ee::LineUniColorTextPack::getFShaderName() const 
{ 
    return "lineUniColor_frag"; 
}
