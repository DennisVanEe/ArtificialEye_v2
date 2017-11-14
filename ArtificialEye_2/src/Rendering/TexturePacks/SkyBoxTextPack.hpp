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
        SkyBoxTextPack(std::string rootDir, const std::vector<std::string>& faceNames);
        SkyBoxTextPack(const SkyBoxTextPack& other) = default;
        SkyBoxTextPack(SkyBoxTextPack&& other);

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) override;
        void postDraw() override {}

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    public:
        CubeMap m_skyBox;
    };
}