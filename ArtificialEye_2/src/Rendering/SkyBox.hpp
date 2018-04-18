#pragma once

#include "Drawable.hpp"
#include "Textures/CubeMap.hpp"

namespace ee
{
    class SkyBox : public Drawable
    {
    public:
        SkyBox(const std::string& textPack);
        SkyBox(const SkyBox& other) = default;
        SkyBox(SkyBox&& other);
        ~SkyBox();

        Drawable* getCopy() const override
        {
            return new SkyBox(*this);
        }

        void draw() override;

    private:
        GLuint m_VAO;
        GLuint m_VBO;
    };
}