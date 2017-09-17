#pragma once

#include <string>
#include "../../Types.hpp"

namespace ee
{
    struct ShaderMaterial
    {
        glm::vec3       m_ambient;
        glm::vec3       m_diffuse;
        glm::vec3       m_specular;
        float           m_shininess;

        std::string     m_textPack;
    };
}