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
        RefractTextPack(glm::vec3 color, std::string rootDir, std::vector<std::string> cubeFaces, float refractIndex);
        RefractTextPack(const RefractTextPack& other) = default;
        RefractTextPack(RefractTextPack&& other);

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) override;
        void postDraw() override {}

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;
        std::string getGShaderName() const override;
        
    public:
        glm::vec3   m_color;
        CubeMap     m_skyBox;
        float       m_refractIndex;
    };
}