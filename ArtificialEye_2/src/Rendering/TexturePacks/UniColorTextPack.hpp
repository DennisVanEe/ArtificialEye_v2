#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"

namespace ee
{
    class UniColorTextPack : public TexturePack
    {
    public:
        UniColorTextPack(glm::vec4 color) : m_color(color) {}

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera) override { shader->assignColor("u_modelColor", m_color); }
        void postDraw() override {}
        TexturePack* getCopy() const override { return new UniColorTextPack(*this); }

        std::string getVShaderName() const override { return "modelUniColor_vert"; }
        std::string getFShaderName() const override { return "modelUniColor_frag"; }

    private:
        glm::vec4 m_color;
    };
}