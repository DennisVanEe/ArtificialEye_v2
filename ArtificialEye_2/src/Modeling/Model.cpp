#include "Model.hpp"

bool ee::Model::loadModel(std::string const file, const ShaderMaterial* const material = nullptr)
{
    bool m_loadMaterial = true;
    if (material == nullptr)
    {
        m_shaderMaterial = ShaderMaterial(*material);
        m_loadMaterial = false;
    }
}