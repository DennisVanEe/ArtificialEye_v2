#pragma once

#include "../Drawable.hpp"
#include "../Textures/Texture.hpp"
#include "Mesh.hpp"

namespace ee
{
    class DrawableMeshContainer : public Drawable
    {
    public:
        DrawableMeshContainer(const Mesh* mesh, const std::string& textPack, bool dynamic = false, int priority = 0);
        ~DrawableMeshContainer()
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_EBO);
            glDeleteBuffers(1, &m_VBO);
        }

        void setTexture(std::vector<Texture> texture) { m_textures = texture; }

        void draw() override;

    private:
        const Mesh* const m_mesh;

        std::vector<FloatVertex> m_cachedVertices;
        std::vector<MeshFace> m_cachedMeshFaces;
        std::vector<Texture> m_textures;

        long long unsigned m_updateCount;

        void castVertexBuffer(const std::vector<Vertex>& vertices);


        GLenum m_type;
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_EBO;
    };
}