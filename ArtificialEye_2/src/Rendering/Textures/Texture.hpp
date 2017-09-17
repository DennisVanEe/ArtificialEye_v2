#pragma once

#include <glad/glad.h>

namespace ee
{
    class Texture
    {
    public:
        GLuint getTexture() const { return m_texture; }
        bool   isEmpty() const { return m_texture == 0; }

    protected:
        Texture(GLuint texture) : m_texture(texture) {}

        GLuint m_texture;
    };
}