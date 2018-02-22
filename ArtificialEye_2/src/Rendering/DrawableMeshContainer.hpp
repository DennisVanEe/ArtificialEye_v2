#pragma once

#include "Drawable.hpp"
#include "Textures/Texture.hpp"
#include "../Mesh/Mesh.hpp"

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
            glDeleteBuffers(1, &m_VBO_vertex);
            glDeleteBuffers(1, &m_VBO_normal);
        }

        void setTexture(std::vector<Texture> texture) { m_textures = texture; }

        void draw() override;

    private:
        const Mesh* const m_mesh;

        std::vector<glm::vec3> m_cachedVertices;
        std::vector<MeshFace> m_cachedMeshFaces;
        std::vector<Texture> m_textures;

        GLenum m_type;
        GLuint m_VAO;
        GLuint m_VBO_vertex;
        GLuint m_VBO_normal;
        GLuint m_EBO;
    };
}