#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"
#include "../Textures/CubeMap.hpp"

namespace ee
{
    class RefractTextPack : public TexturePack
    {
    public:
        RefractTextPack(Vector3 color, std::string rootDir, std::vector<std::string> cubeFaces, float refractIndex);

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera) override;
        void postDraw() override {}
        TexturePack* getCopy() const override;

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;
        std::string getGShaderName() const override;
        
    public:
        Color3 m_color;
        CubeMap m_skyBox;
        float m_refractIndex;
    };
}