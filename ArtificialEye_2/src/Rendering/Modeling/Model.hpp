#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Mesh.hpp"

namespace ee
{
    enum class LoadFlags {IGNORE_MATERIAL};

    class Model
    {
    public:
        Model();
        Model(Model&& model);

        bool loadModel(std::string file, LoadFlags flags, const ShaderMaterial* material = nullptr);

        void draw();

    private:
        std::vector<std::unique_ptr<Mesh>> m_meshList;
    };
}