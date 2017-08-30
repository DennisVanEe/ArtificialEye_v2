#pragma once

#include <string>
#include "../../Types.hpp"

namespace ee
{
    struct ShaderMaterial
    {
        Vector3 m_ambient;
        Vector3 m_diffuse;
        Vector3 m_specular;
        Float m_shininess;

        std::string m_textPack;
    };
}