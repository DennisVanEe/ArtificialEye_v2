#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.hpp"
#include "DrawableMeshContainer.hpp"
#include "../Textures/Texture.hpp"
#include "../shader.hpp"
#include "../Drawable.hpp"
#include "../TexturePacks/EyeballTextPack.hpp"

namespace ee
{
    class LoadableModel
    {
    public:
        LoadableModel(std::string dir, int priority) :
            m_textPack("ModelTextPack" + m_numModel), m_priority(priority), m_dir(dir)
        {
            m_numModel++;
            void* res = Renderer::addTexturePack(m_textPack, EyeballTextPack());
            if (res == nullptr)
            {
                throw std::runtime_error("Issue loading texture pack for model.");
            }
        }

        void setTransform(const Mat4& transform)
        {
            for (auto& mesh : m_meshes)
            {
                mesh->setModelTrans(transform);
            }
        }

        void load();

    private:
        void processMesh(const aiMesh* mesh, const aiScene* scene, bool* check = nullptr);
        void processNode(aiNode* node, const aiScene* scene, bool* check = nullptr);
        std::vector<Texture> loadTexture(const aiMaterial* mat, aiTextureType type, TextType typeName, bool* check = nullptr);

        // The mesh data
        std::vector<std::vector<Texture>> m_textures;
        std::vector<std::unique_ptr<Mesh>> m_meshes;
        std::vector<std::unique_ptr<DrawableMeshContainer>> m_drawables;
        std::string m_dir;
        std::string m_textPack;
        int m_priority;

        static int m_numModel;
    };
}