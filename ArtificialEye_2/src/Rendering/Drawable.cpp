#include "Drawable.hpp"

#include <sstream>

ee::Drawable::Drawable(std::string textPack, int priority) :
    m_texturePack(Renderer::getTexturePack(textPack)),
    m_priority(priority)
{
    if (priority < 0 && priority != RENDER_FIRST)
    {
        m_priority = 0;
    }

    if (m_texturePack == nullptr)
    {
        std::stringstream strstrm;
        strstrm << "Unable to load or compile the shaders: " << textPack;
        throw std::runtime_error(strstrm.str());
    }

    m_shader = ee::Renderer::loadShader(m_texturePack->getVShaderName(), m_texturePack->getFShaderName(), m_texturePack->getGShaderName());
    if (m_shader == nullptr)
    {
        std::stringstream strstrm;
        strstrm << "Unable to load or compile the shaders: " << m_texturePack->getVShaderName() << ", " << m_texturePack->getFShaderName() << 
            " and " << m_texturePack->getGShaderName();
        throw std::runtime_error(strstrm.str());
    }
}

ee::Drawable::Drawable(const std::string vertName, const std::string fragName) :
    m_shader(Renderer::loadShader(vertName, fragName)),
    m_texturePack(nullptr)
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

void ee::Drawable::setShaderMaterial(ShaderMaterial mat)
{
    m_shaderMaterial = mat;
}