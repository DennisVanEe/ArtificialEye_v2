#pragma once

#include "../Drawable.hpp"
#include "../TexturePacks/Textures/CubeMap.hpp"

namespace ee
{
    class SkyBox : public Drawable
    {
    public:
        SkyBox(std::string textPack, std::string rootDir, std::vector<std::string> cubeFaces);

        void draw() override;

    private:
        static const float m_vertices[];

        GLuint m_VAO;
        CubeMap m_cubeMap;
    };
}