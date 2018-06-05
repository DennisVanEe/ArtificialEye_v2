#pragma once

#include "EyeballTextPack.hpp"

#include <iostream>

ee::EyeballTextPack::EyeballTextPack(glm::vec4 color) :
    m_color(color)
{
}

void ee::EyeballTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    shader->assignColor("u_modelColor", m_color);
}

std::string ee::EyeballTextPack::getVShaderName() const
{
    return "eyeball_vert";
}

std::string ee::EyeballTextPack::getFShaderName() const
{
    return "eyeball_frag";
}