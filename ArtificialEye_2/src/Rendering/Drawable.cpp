#include "Drawable.hpp"

#include <sstream>

ee::Drawable::Drawable(const std::string textPack) :
    m_texturePack(Renderer::getTexturePack(textPack))
{
    if (m_texturePack == nullptr)
    {
        std::stringstream strstrm;
        strstrm << "Unable to load or compile the shaders: " << textPack;
        throw std::runtime_error(strstrm.str());
    }

    m_shader = ee::Renderer::loadShader(m_texturePack->getVShaderName(), m_texturePack->getFShaderName());
    if (m_shader == nullptr)
    {
        std::stringstream strstrm;
        strstrm << "Unable to load or compile the shaders: " << m_texturePack->getVShaderName() << " and " << m_texturePack->getFShaderName();
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