#include "Model.hpp"

bool ee::Model::loadModel(std::string const file, LoadFlags const flags, const ShaderMaterial* const material = nullptr)
{
    if (flags == LoadFlags::IGNORE_MATERIAL)
    {
        if (material == nullptr)
        {
            return false;
        }


    }
}
