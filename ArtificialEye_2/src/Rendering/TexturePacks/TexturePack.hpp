#pragma once

#include <string>
#include "ShaderMaterial.hpp"
#include "../Camera.hpp"
#include "../Textures/Texture.hpp"

namespace ee
{
    class Shader;

    class TexturePack
    {
    public:
        virtual void preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures = nullptr) = 0;
        virtual void postDraw() = 0;

        virtual std::string getVShaderName() const = 0;
        virtual std::string getFShaderName() const = 0;
        virtual std::string getGShaderName() const { return ""; }
    };
}