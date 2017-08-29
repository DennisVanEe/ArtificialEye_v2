#include "RefractTextPack.hpp"

ee::RefractTextPack::RefractTextPack(Vector3 color, std::string rootDir, std::vector<std::string> cubeFaces, float refractIndex) :
    m_color(color),
    m_skyBox(loadCubeMap(rootDir, cubeFaces)),
    m_refractIndex(refractIndex)
{
}

void ee::RefractTextPack::setTexture(Shader* shader, const ShaderMaterial* material, const Camera* camera)
{
    shader->assignColor("u_modelColor", m_color); // assign the color
    Vector3 camPos = camera->getPosition();
    shader->assignVec3f("u_cameraPosition", camPos);
    shader->assignFloat("u_refractIndex", m_refractIndex);
    shader->bindTexture(GL_TEXTURE_CUBE_MAP, 0, m_skyBox.getTexture());
}

ee::TexturePack* ee::RefractTextPack::getCopy() const
{
    return new RefractTextPack(*this);
}

std::string ee::RefractTextPack::getVShaderName() const
{
    return "modelRefract_vert";
}

std::string ee::RefractTextPack::getFShaderName() const
{
    return "modelRefract_frag.glsl";
}