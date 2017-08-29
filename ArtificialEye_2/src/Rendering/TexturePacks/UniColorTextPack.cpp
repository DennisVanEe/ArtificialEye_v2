#include "UniColorTextPack.hpp"

ee::UniColorTextPack::UniColorTextPack(Color4 color) :
    m_color(color)
{
}

void ee::UniColorTextPack::setTexture(Shader* shader)
{
    shader->assignColor("u_modelColor", m_color);
}

ee::TexturePack* ee::UniColorTextPack::getCopy() const
{
    return new UniColorTextPack(*this);
}

bool ee::UniColorTextPack::usesMaterial() const
{
    return false;
}

std::string ee::UniColorTextPack::getVShaderName() const
{
    return "modelUniColor_vert";
}

std::string ee::UniColorTextPack::getFShaderName() const
{
    return "modelUniColor_frag";
}