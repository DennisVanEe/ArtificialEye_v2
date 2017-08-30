#pragma once

#include <string>
#include <numeric>
#include "TexturePacks/TexturePack.hpp"
#include "Renderer.hpp"
#include "TexturePacks/ShaderMaterial.hpp"

namespace ee
{
    // annoying Windows.h macros...
    const int RENDER_LAST = (std::numeric_limits<int>::max)();
    const int RENDER_FIRST = (std::numeric_limits<int>::min)();

    class Drawable
    {
    public:
        Drawable(std::string textPack, int priority);
        Drawable(std::string vertName, std::string fragName);
        Drawable(const Drawable& drawable);

        void setShaderMaterial(ShaderMaterial mat);

        int getPriority() const { return m_priority; }

        virtual void draw() = 0;

    protected:
        TexturePack* m_texturePack;
        Shader* m_shader;
        ShaderMaterial m_shaderMaterial;
        int m_priority;
    };

    struct DrawableCompare
    {
        bool operator()(Drawable* a, Drawable* b) const
        {
            return a->getPriority() < b->getPriority();
        }
    };
}