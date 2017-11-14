#include "RefractTextPack.hpp"

ee::RefractTextPack::RefractTextPack(glm::vec3 color, std::string rootDir, std::vector<std::string> cubeFaces, float refractIndex) :
    m_color(color),
    m_skyBox(loadCubeMap(rootDir, cubeFaces)),
    m_refractIndex(refractIndex)
{
}

ee::RefractTextPack::RefractTextPack(RefractTextPack&& other) : 
    m_color(other.m_color), 
    m_skyBox(std::move(other.m_skyBox)), 
    m_refractIndex(other.m_refractIndex) 
{
}

void ee::RefractTextPack::preDraw(Shader* shader, const ShaderMaterial* material, const Camera* camera, const std::vector<Texture>* textures)
{
    // shader->assignColor("u_modelColor", m_color); // assign the color
    glm::vec3 camPos = camera->getPosition();
    shader->assignVec3("u_cameraPosition", camPos);
    shader->assignfloat("u_refractIndex", m_refractIndex);
    shader->bindTexture(GL_TEXTURE_CUBE_MAP, 0, m_skyBox.getTexture());
}

std::string ee::RefractTextPack::getVShaderName() const
{
    return "modelRefract_vert";
}

std::string ee::RefractTextPack::getFShaderName() const
{
    return "modelRefract_frag";
}

std::string ee::RefractTextPack::getGShaderName() const
{
    return "modelRefract_geom";
}