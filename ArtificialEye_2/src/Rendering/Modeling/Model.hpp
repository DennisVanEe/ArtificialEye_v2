#pragma once

#include <vector>
#include <memory>
#include <string>

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include "../../Rendering/TexturePacks/ShaderMaterial.hpp"
//#include "../../Rendering/TexturePacks/Texture.hpp"
//
//#include "Mesh.hpp"
//#include "../Rendering/TexturePacks/ShaderMaterial.hpp"

namespace ee
{
    enum class LoadFlags {DYNAMIC, STATIC};

    template<class MeshType>
    class Model
    {
    public:
        Model();
        Model(Model&& model);

        bool loadModel(std::string file, const ShaderMaterial* material = nullptr);

        void draw();

    private:
        MeshType processMesh(const aiMesh* mesh, const aiScene* scene, const ShaderMaterial* material, bool loadMat, bool* check = nullptr);
        void processNode(aiNode* node, const aiScene* scene, const ShaderMaterial* material, bool loadMat, bool* check = nullptr);

        struct MeshBucket
        {
            MeshType m_mesh;
            ShaderMaterial m_material;
        };

        std::vector<MeshBucket> m_meshList;
        std::vector<std::unique_ptr<Mesh>> m_meshList;
        ShaderMaterial m_shaderMaterial;
    };
}

// template definitions:

template<class MeshType>
void ee::Model<MeshType>::processNode(aiNode* const node, const aiScene* const scene, const ShaderMaterial* material, bool const loadMat, bool* const check)
{
    bool correctLoad;
    for (std::size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshList.push_back(processMesh(mesh, scene, material, loadMat, &correctLoad));
        if (!correctLoad && check)
        {
            *check = false;
        }
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, &correctLoad);
        if (!correctLoad && check)
        {
            *check = false;
        }
    }

    *check = true;
}

template<class MeshType>
MeshType ee::Model<MeshType>::processMesh(const aiMesh* mesh, const aiScene* scene, const ShaderMaterial* material, const bool loadMat, bool* check)
{
    VertBuffer tempVert;
    MeshFaceBuffer tempInd;
    std::vector<Texture> tempTexts;

    MeshBucket result;

    // push them vertices
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.m_position.x = mesh->mVertices[i].x;
        vertex.m_position.y = mesh->mVertices[i].y;
        vertex.m_position.z = mesh->mVertices[i].z;

        vertex.m_normal.x = mesh->mNormals[i].x;
        vertex.m_normal.z = mesh->mNormals[i].y;
        vertex.m_normal.z = mesh->mNormals[i].z;

        if (loadMat && mesh->mTextureCoords[0])
        {
            vertex.m_textCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.m_textCoord.y = mesh->mTextureCoords[0][i].y;
        }

        tempVert.push_back(vertex);
    }

    // push them indices
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace* face = &mesh->mFaces[i];
        for (size_t k = 0; k < face->mNumIndices; k++)
        {
            tempInd.push_back(face->mIndices[k]);
        }
    }

    // TODO: load actual material:
    if (loadMat)
    {
        bool correctLoad;
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture> diffuse = loadTexture(material, aiTextureType_DIFFUSE, TextType::DIFFUSE, &correctLoad);
            if (!correctLoad && check)
            {
                *check = false;
                return Mesh();
            }
            tempTexts.insert(tempTexts.end(), diffuse.begin(), diffuse.end());
            std::vector<Texture> specular = loadTexture(material, aiTextureType_SPECULAR, TextType::SPECULAR, &correctLoad);
            if (!correctLoad && check)
            {
                *check = false;
                return Mesh();
            }
            tempTexts.insert(tempTexts.end(), specular.begin(), specular.end());
        }
    }
    else
    {
        result.m_material = ShaderMaterial(*material);
    }

    result.m_mesh = MeshType(result.m_material.m_textPack, tempVert, tempInd);
    *check = true;
    return std::move(result);
}

template<class MeshType>
bool ee::Model<MeshType>::loadModel(std::string const file, const ShaderMaterial* const material = nullptr)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "[ERROR] Opening or processing model at " << file << std::endl;
        std::cout << "Message: " << importer.GetErrorString() << std::endl;
        return false;
    }

    // TODO: textures:
    /* std::string textureLoc;
    auto pos = file.find_last_of('/');
    if (pos == std::string::npos)
    {
    textureLoc = "/";
    }
    else
    {
    textureLoc = file.substr(0, pos) + "/";
    }*/

    aiNode* node = scene->mRootNode;
    bool loadCheck;
    processNode(node, scene, &loadCheck);
    if (!loadCheck)
    {
        std::cout << "[ERROR] processing model at " << file << std::endl;
        return false;
    }

    return true;
}

template<class MeshType>
void ee::Model<MeshType>::draw()
{

}