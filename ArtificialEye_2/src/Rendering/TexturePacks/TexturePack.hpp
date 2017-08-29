#pragma once

#include <string>
#include "ShaderMaterial.hpp"
#include "../Camera.hpp"

namespace ee
{
    class Shader;

    class TexturePack
    {
    public:
        virtual void setTexture(Shader* shader, const ShaderMaterial* material, const Camera* camera) = 0;
        virtual TexturePack* getCopy() const = 0;

        virtual std::string getVShaderName() const = 0;
        virtual std::string getFShaderName() const = 0;
    };
}