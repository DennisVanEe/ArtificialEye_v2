#pragma once

#include "../Drawable.hpp"
#include "../Textures/CubeMap.hpp"

namespace ee
{
    class SkyBox : public Drawable
    {
    public:
        SkyBox(std::string textPack);

        void draw() override;

    private:
        static const float m_vertices[];

        GLuint m_VAO;
    };
}