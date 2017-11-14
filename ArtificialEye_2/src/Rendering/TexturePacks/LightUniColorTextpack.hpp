#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"
#include "ShaderMaterial.hpp"

namespace ee
{
    class LightUniColorTextPack : public TexturePack
    {
    public:
        LightUniColorTextPack();
        LightUniColorTextPack(glm::vec3 lightPosition, glm::vec3 color, ShaderMaterial material);

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) override;
        void postDraw() override {}

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    public:
        glm::vec3 m_lightPosition;
        glm::vec3 m_color;
        ShaderMaterial m_material;
    };
}