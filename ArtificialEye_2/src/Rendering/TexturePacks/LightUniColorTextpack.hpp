#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"

namespace ee
{
    class LightUniColorTextPack : public TexturePack
    {
    public:
        LightUniColorTextPack(Vector3 lightPosition, Vector3 color, ShaderMaterial material);
        LightUniColorTextPack();

        void setTexture(Shader* shader) override;
        TexturePack* getCopy() const override;

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    public:
        Vector3 m_lightPosition;
        Color3 m_color;
        ShaderMaterial m_material;
    };
}