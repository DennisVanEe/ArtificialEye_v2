#pragma once

#include <string>
#include <numeric>

#include "TexturePacks/TexturePack.hpp"
#include "Renderer.hpp"
#include "TexturePacks/ShaderMaterial.hpp"

namespace ee
{
    // annoying Windows.h macros...
    const int RENDER_LAST =  INT_MAX;
    const int RENDER_FIRST = INT_MIN;

    class Drawable
    {
    public:
        Drawable(const std::string& textPack, int priority);
        Drawable(const std::string& vertName, const std::string& fragName);

        void setShaderMaterial(ShaderMaterial mat);

        int getPriority() const;

        bool willDraw() const;

        virtual void draw() = 0;

    protected:
        TexturePack*    m_texturePack;
        Shader*         m_shader;
        ShaderMaterial  m_shaderMaterial;
        int             m_priority;
    };

    struct DrawableCompare
    {
        bool operator()(const Drawable* a, const Drawable* b) const;
    };
}