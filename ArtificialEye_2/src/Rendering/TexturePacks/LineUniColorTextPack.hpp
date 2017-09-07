#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"
#include "ShaderMaterial.hpp"

namespace ee
{
    class LineUniColorTextPack : public TexturePack
    {
    public:
        LineUniColorTextPack(Color3 color) : m_color(color) {}
        LineUniColorTextPack() : m_color(1.f, 1.f, 1.f) {}

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera) override
        {
            shader->assignColor("u_color", m_color);
        }

        void postDraw() override {}
        TexturePack* getCopy() const override { return new LineUniColorTextPack(*this); }

        std::string getVShaderName() const override { return "lineUniColor_vert"; }
        std::string getFShaderName() const override { return "lineUniColor_frag"; }

    public:
        Color3 m_color;
    };
}