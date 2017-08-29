#pragma once

#include <string>
#include "TexturePacks/TexturePack.hpp"
#include "Renderer.hpp"
#include "TexturePacks/ShaderMaterial.hpp"

namespace ee
{
    class Drawable
    {
    public:
        Drawable(std::string textPack);
        Drawable(std::string vertName, std::string fragName);
        Drawable(const Drawable& drawable);

        void setShaderMaterial(ShaderMaterial mat);

        virtual void draw() = 0;

    protected:
        TexturePack* m_texturePack;
        Shader* m_shader;
        ShaderMaterial m_shaderMaterial;
    };
}