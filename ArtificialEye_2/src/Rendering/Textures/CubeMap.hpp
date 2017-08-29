#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace ee
{
    class CubeMap
    {
    public:
        ~CubeMap()
        {
            glDeleteTextures(1, &m_texture);
        }

        GLuint getTexture() const
        {
            return m_texture;
        }

    private:
        CubeMap(GLuint texture);
        friend CubeMap loadCubeMap(std::string, std::vector<std::string>);

        GLuint m_texture;
    };

    CubeMap loadCubeMap(std::string rootDir, std::vector<std::string> mapFaces);
}