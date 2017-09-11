#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/epsilon.hpp>

namespace ee
{
    const char PROJ_NAME[] = "ArtificalEye";

    using Color4 = glm::vec4;
    using Color3 = glm::vec3;
    using Vector3 = glm::vec3;

    glm::vec3 removeZeroError(glm::vec3 vec);
}