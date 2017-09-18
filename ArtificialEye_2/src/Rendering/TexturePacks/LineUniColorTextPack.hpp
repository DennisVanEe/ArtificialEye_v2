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
        LineUniColorTextPack(glm::vec3 color);
        LineUniColorTextPack();

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera) override;

        void postDraw() override;

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    public:
        glm::vec3 m_color;
    };
}