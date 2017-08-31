#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ee
{
    const char PROJ_NAME[] = "ArtificalEye";

    using Color4 = glm::vec4;
    using Color3 = glm::vec3;
    using Vector3 = glm::vec3;

    struct MeshFace
    {
        GLuint m_indices[3];
    };
    static_assert(sizeof(MeshFace) == 3 * sizeof(GLuint), "MeshFace can't have anny padding");

    //Color3 Color3i(unsigned char r, unsigned char g, unsigned char b)
    //{
    //    return Color3(r / 255.f, g / 255.f, b / 255.f);
    //}

    //Color4 Color4i(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    //{
    //    return Color4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    //}
}