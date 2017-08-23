#pragma once

#include "TexturePack.hpp"
#include "Shader.hpp"
#include "../Types.hpp"

namespace ee
{
    class UniColorTextPack : public TexturePack
    {
    public:
        UniColorTextPack(Color4 color);

        void setTexture(Shader* shader) override;
        TexturePack* getCopy() const override;

        std::string getVShaderName() const override;
        std::string getFShaderName() const override;

    private:
        Color4 m_color;
    };
}