#include "Drawable.hpp"

#include <sstream>

ee::Drawable::Drawable(const std::string vertName, const std::string fragName) :
    m_shader(Renderer::loadShader(vertName, fragName))
{
    if (m_shader == nullptr)
    {
        std::stringstream strstrm;
        strstrm << "Unable to load or compile the shaders: " << vertName << " and " << fragName;
        throw std::runtime_error(strstrm.str());
    }
}

ee::Drawable::Drawable(const Drawable& drawable) :
    m_shader(drawable.m_shader)
{
}
