#pragma once

#include <string>
#include "Shader.hpp"
#include "Renderer.hpp"

namespace ee
{
    class Drawable
    {
    public:
        Drawable(std::string vertName, std::string fragName);
        Drawable(const Drawable& drawable);

        virtual void draw() = 0;

    protected:
        Shader* const m_shader;
    };
}