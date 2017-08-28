#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Mesh.hpp"
#include "../Rendering/TexturePacks/ShaderMaterial.hpp"

namespace ee
{
    class Model
    {
    public:
        Model();
        Model(Model&& model);

        bool loadModel(std::string file, const ShaderMaterial* material = nullptr);

        void draw();

    private:
        std::vector<std::unique_ptr<Mesh>> m_meshList;
        ShaderMaterial m_shaderMaterial;
    };
}