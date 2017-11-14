#pragma once

#include <glad/glad.h>
#include <string>

#include "../../STBI/stb_image.hpp"

namespace ee
{
    enum class TextType { DIFFUSE, SPECULAR, NONE };

    class Texture
    {
    public:
        Texture(const std::string& name, const std::string& path);
        Texture(GLuint texture) : m_texture(texture) {}
        Texture(const Texture& other) = default;
        Texture(Texture&& other);

        GLuint getTexture() const { return m_texture; }
        bool   isEmpty() const { return m_texture == 0; }

        TextType m_type;

    private:
        GLuint m_texture;
    };
}