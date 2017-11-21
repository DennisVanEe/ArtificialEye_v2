#include "LoadableModel.hpp"

#include <iostream>

#include "../Subdivision.hpp"
#include "../../Initialization.hpp"

int ee::LoadableModel::m_numModel = 0;

const int corneaVertexCount = 352;
const int pupilVertexCount = 224;
// 224 pupil
// 128 iris
// 352 cornea
// 1632

void ee::LoadableModel::load()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(m_dir, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    auto pos = m_dir.find_last_of('/');
    if (pos == std::string::npos)
    {
        m_dir = "/";
    }
    else
    {
        m_dir = m_dir.substr(0, pos) + "/";
    }

    aiNode* node = scene->mRootNode;
    bool loadCheck;
    processNode(node, scene, &loadCheck);
    if (!loadCheck)
    {
        throw std::runtime_error("Issue when parsing and setting up nodes.");
    }

    std::cout << m_meshes.size() << std::endl;
    for (int i = 0; i < m_meshes.size(); i++)
    {
        if (pupilVertexCount == m_meshes[i]->getNumVertices()) { continue; }
        m_drawables.push_back(std::unique_ptr<DrawableMeshContainer>(new DrawableMeshContainer(m_meshes[i].get(), m_textPack, false)));
        m_drawables.back()->setTexture(m_textures[i]);
    }

    for (auto& drawable : m_drawables)
    {
        Renderer::addDrawable(drawable.get());
    }
}

std::vector<ee::Texture> ee::LoadableModel::loadTexture(const aiMaterial* mat, aiTextureType type, TextType typeName, bool* check)
{
    std::vector<Texture> textures;
    for (size_t i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture(str.C_Str(), m_dir);

        if (texture.getTexture() == 0 && check)
        {
            *check = false;
            return std::vector<Texture>();
        }

        texture.m_type = typeName;
        textures.push_back(texture);
    }

    *check = true;
    return textures;
}

void ee::LoadableModel::processMesh(const aiMesh* mesh, const aiScene* scene, bool* check)
{
    std::vector<Vertex> tempVert;
    std::vector<MeshFace> tempInd;
    std::vector<Texture> tempTexts;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.m_position.x = mesh->mVertices[i].x;
        vertex.m_position.y = mesh->mVertices[i].y;
        vertex.m_position.z = mesh->mVertices[i].z;

        vertex.m_normal.x = mesh->mNormals[i].x;
        vertex.m_normal.y = mesh->mNormals[i].y;
        vertex.m_normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.m_textCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.m_textCoord.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.m_textCoord = ee::Vec2(0.0, 0.0);
        }

        tempVert.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace* face = &mesh->mFaces[i];
        for (int k = 0; k < face->mNumIndices;)
        {
            // this SHOULD be safe because there should be at a multiple of 3 vertices
            MeshFace newFace({ static_cast<int>(face->mIndices[k++]),
                static_cast<int>(face->mIndices[k++]), static_cast<int>(face->mIndices[k++]) });
            tempInd.push_back(newFace);
        }
    }

    bool correctLoad;
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuse = loadTexture(material, aiTextureType_DIFFUSE, TextType::DIFFUSE, &correctLoad);
        if (!correctLoad && check)
        {
            *check = false;
        }
        tempTexts.insert(tempTexts.end(), diffuse.begin(), diffuse.end());

        std::vector<Texture> specular = loadTexture(material, aiTextureType_SPECULAR, TextType::SPECULAR, &correctLoad);
        if (!correctLoad && check)
        {
            *check = false;
        }
        tempTexts.insert(tempTexts.end(), specular.begin(), specular.end());
    }

    // A hack, but I got to get this done now
    if (tempVert.size() == corneaVertexCount)
    {
        Mesh temp(tempVert, tempInd);
        temp = loopSubdiv(temp, ARTIFICIAL_EYE_PROP.subdiv_level_cornea);
        tempVert = std::move(temp.getVerticesData());
        tempInd = std::move(temp.getMeshFaceData());
    }

    m_meshes.push_back(std::unique_ptr<Mesh>(new Mesh(tempVert, tempInd)));
    m_textures.push_back(std::move(tempTexts));

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