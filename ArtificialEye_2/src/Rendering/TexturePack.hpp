#pragma once

#include <string>

namespace ee
{
    class Shader;

    class TexturePack
    {
    public:
        virtual void setTexture(Shader* shader) = 0;
        virtual TexturePack* getCopy() const = 0;

        virtual std::string getVShaderName() const = 0;
        virtual std::string getFShaderName() const = 0;
    };
}