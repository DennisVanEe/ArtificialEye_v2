#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "../Rendering/DrawableMeshContainer.hpp"
#include "../Rendering/Textures/Texture.hpp"
#include "../Rendering/shader.hpp"
#include "../Rendering/Drawable.hpp"
#include "../Rendering/TexturePacks/EyeballTextPack.hpp"

namespace ee
{
    class LoadableModel
    {
    public:
        void load(std::string dir);
        const std::vector<Mesh> getMeshes() const
        {
            return m_meshes;
        }

        std::vector<Mesh>& getMeshes()
        {
            return m_meshes;
        }

    private:
        void processMesh(const aiMesh* mesh, const aiScene* scene, bool* check = nullptr);
        void processNode(aiNode* node, const aiScene* scene, bool* check = nullptr);

        // The mesh data:
        std::vector<Mesh> m_meshes;
        std::string m_dir;
    };
}