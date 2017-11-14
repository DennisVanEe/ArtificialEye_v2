#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"
#include "ShaderMaterial.hpp"

namespace ee
{
    class EyeballTextPack : public TexturePack
    {
    public:
        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) override;

        void postDraw() override {}

        std::string getVShaderName() const override { return "eyeball_vert"; }
        std::string getFShaderName() const override { return "eyeball_frag"; }

    public:
        glm::vec3 m_lightPosition;
        glm::vec3 m_color;
        ShaderMaterial m_material;
    };
}
