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

    struct Ray
    {
        glm::vec3 m_origin;
        glm::vec3 m_dir;

        Ray() = default;
        Ray(glm::vec3 origin, glm::vec3 dir);
    };

    struct Line
    {
        glm::vec3 m_start;
        glm::vec3 m_end;

        Line() = default;
        Line(glm::vec3 start, glm::vec3 end);
    };

    glm::vec3 removeZeroError(glm::vec3 vec);
    glm::vec3 transPoint3(const glm::mat4& mat, glm::vec3 vec);
    glm::vec3 transVector3(const glm::mat4& mat, glm::vec3 vec);
}