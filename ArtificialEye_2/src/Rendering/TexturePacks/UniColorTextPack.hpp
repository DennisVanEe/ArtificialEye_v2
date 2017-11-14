#pragma once

#include "TexturePack.hpp"
#include "../Shader.hpp"
#include "../../Types.hpp"

namespace ee
{
    class UniColorTextPack : public TexturePack
    {
    public:
        UniColorTextPack(glm::vec4 color);

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) override;
        void postDraw() override {}

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    private:
        glm::vec4 m_color;
    };
}