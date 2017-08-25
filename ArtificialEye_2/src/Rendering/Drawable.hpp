#pragma once

#include <string>
#include "TexturePacks/TexturePack.hpp"
#include "Renderer.hpp"

namespace ee
{
    class Drawable
    {
    public:
        Drawable(std::string textPack);
        Drawable(std::string vertName, std::string fragName);
        Drawable(const Drawable& drawable);

        virtual void draw() = 0;

    protected:
        TexturePack* m_texturePack;
        Shader* m_shader;
    };
}