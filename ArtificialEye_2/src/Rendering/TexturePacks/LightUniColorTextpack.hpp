#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"

namespace ee
{
    struct ShaderMaterial
    {
        Vector3 m_ambient;
        Vector3 m_diffuse;
        Vector3 m_specular;
        Float m_shininess;
    };

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