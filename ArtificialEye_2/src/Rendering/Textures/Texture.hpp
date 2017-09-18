#pragma once

#include <glad/glad.h>

namespace ee
{
    class Texture
    {
    public:
        Texture(const Texture& other) = default;
        Texture(Texture&& other);

        GLuint getTexture() const { return m_texture; }
        bool   isEmpty() const { return m_texture == 0; }

    protected:
        Texture(GLuint texture) : m_texture(texture) {}

    private:
        GLuint m_texture;
    };
}