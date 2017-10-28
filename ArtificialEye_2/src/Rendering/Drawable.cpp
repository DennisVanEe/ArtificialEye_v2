#include "Drawable.hpp"

#include <sstream>

ee::Drawable::Drawable(const std::string& textPack, const int priority) :
    m_texturePack(Renderer::getTexturePack(textPack)),
    m_priority(priority)
{
    if (m_texturePack == nullptr)
    {
        return; // it won't be rendered (this is kind of a hack, but it is quick)
    }

    if (m_priority < 0 && m_priority != RENDER_FIRST)
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

ee::Drawable::Drawable(const std::string& vertName, const std::string& fragName) :
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

void ee::Drawable::setShaderMaterial(ShaderMaterial mat) 
{ 
    m_shaderMaterial = mat; 
}

int ee::Drawable::getPriority() const 
{ 
    return m_priority; 
}

bool ee::Drawable::willDraw() const
{
    return m_texturePack != nullptr;
}

bool ee::DrawableCompare::operator()(const Drawable* a, const Drawable* b) const
{
    return a->getPriority() < b->getPriority();
}
