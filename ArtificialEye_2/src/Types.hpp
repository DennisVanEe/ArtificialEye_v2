#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/epsilon.hpp>

namespace ee
{
    const char PROJ_NAME[] = "ArtificalEye";

    const float PI  = glm::pi<float>();
    const float PI2 = 2 * PI;

    glm::vec3 removeZeroError(glm::vec3 vec);
}