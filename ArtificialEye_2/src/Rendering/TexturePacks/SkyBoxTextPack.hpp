#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"
#include "../Textures/CubeMap.hpp"

namespace ee
{
    class SkyBoxTextPack : public TexturePack
    {
    public:
        SkyBoxTextPack(CubeMap skyBox);

        void setTexture(Shader* shader, const ShaderMaterial* material, const Camera* camera) override;
        TexturePack* getCopy() const override;

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    public:
        CubeMap m_skyBox;
    };
}