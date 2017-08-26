#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ee
{
#ifdef USE_DOUBLE
    using Float = double;
#   define F(x) static_cast<double>(x)
#else
    using Float = float;
#   define F(x) static_cast<float>(x)
#endif

    const char PROJ_NAME[] = "ArtificalEye";

    using Color3 = glm::tvec3<float>;
    using Color4 = glm::tvec4<float>;
    using Vector3 = glm::tvec3<Float>;

    struct ShaderMaterial
    {
        Vector3 m_ambient;
        Vector3 m_diffuse;
        Vector3 m_specular;
        Float m_shininess;
    };
}