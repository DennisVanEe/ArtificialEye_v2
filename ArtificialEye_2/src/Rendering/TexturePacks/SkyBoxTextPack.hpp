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
        SkyBoxTextPack(std::string rootDir, const std::vector<std::string>& faceNames) : m_skyBox(loadCubeMap(rootDir, faceNames)) {}

        void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera) override { shader->bindTexture(GL_TEXTURE_CUBE_MAP, 0, m_skyBox.getTexture()); }
        void postDraw() override {}
        TexturePack* getCopy() const override { return new SkyBoxTextPack(*this); }

        std::string getVShaderName() const override { return "skyBox_vert"; }
        std::string getFShaderName() const override { return "skyBox_frag"; }

    public:
        CubeMap m_skyBox;
    };
}