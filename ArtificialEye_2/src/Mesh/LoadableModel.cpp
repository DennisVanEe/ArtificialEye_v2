#include "LoadableModel.hpp"

#include <iostream>

#include "../Mesh/Subdivision.hpp"
#include "../Initialization.hpp"

const int corneaVertexCount = 352;
const int pupilVertexCount = 224;
// 224 pupil
// 128 iris
// 352 cornea
// 1632

void ee::LoadableModel::load(std::string dir)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(dir, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    m_dir = dir;

    aiNode* node = scene->mRootNode;
    bool loadCheck;
    processNode(node, scene, &loadCheck);
    if (!loadCheck)
    {
        throw std::runtime_error("Issue when parsing and setting up nodes.");
    }
}

void ee::LoadableModel::processMesh(const aiMesh* mesh, const aiScene* scene, bool* check)
{
    std::vector<glm::vec3> tempVert;
    std::vector<MeshFace> tempInd;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 vertex;
        vertex.x = mesh->mVertices[i].x;
        vertex.y = mesh->mVertices[i].y;
        vertex.z = mesh->mVertices[i].z;

        tempVert.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace* face = &mesh->mFaces[i];
        for (int k = 0; k < face->mNumIndices; k += 3)
        {
            int f0 = face->mIndices[k];
            int f1 = face->mIndices[k + 1];
            int f2 = face->mIndices[k + 2];
            // this SHOULD be safe because there should be at a multiple of 3 vertices
            MeshFace newFace({f0, f1, f2});
            tempInd.push_back(newFace);
        }
    }

    Mesh temp;
    temp.create(tempVert, tempInd);
    m_meshes.push_back(temp);

    *check = true;
}

void ee::LoadableModel::processNode(aiNode* node, const aiScene* scene, bool* check)
{
    bool correctLoad;
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, &correctLoad);
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