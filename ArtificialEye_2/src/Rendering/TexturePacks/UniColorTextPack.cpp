#include "UniColorTextPack.hpp"

ee::UniColorTextPack::UniColorTextPack(glm::vec4 color) : 
    m_color(color) 
{
}

void ee::UniColorTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{ 
    shader->assignColor("u_modelColor", m_color); 
}

std::string ee::UniColorTextPack::getVShaderName() const 
{ 
    return "modelUniColor_vert"; 
}

std::string ee::UniColorTextPack::getFShaderName() const
{
    return "modelUniColor_frag";
}
