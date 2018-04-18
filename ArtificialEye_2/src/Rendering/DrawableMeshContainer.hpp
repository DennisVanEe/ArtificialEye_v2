#pragma once

#include "Drawable.hpp"
#include "Textures/Texture.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
    class DrawableMeshContainer : public Drawable
    {
    public:
        DrawableMeshContainer(const Mesh* mesh, const std::string& textPack, bool dynamic = false, bool sendNormals = false, bool sendTextCoords = false, int priority = 0);
        ~DrawableMeshContainer()
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_EBO);
            glDeleteBuffers(1, &m_VBO_vertex);
            glDeleteBuffers(1, &m_VBO_normal);
        }

        void setTexture(std::vector<Texture> texture) { m_textures = texture; }

        Drawable* getCopy() const override
        {
            return new DrawableMeshContainer(*this);
        }

        void draw() override;

    private:
        const Mesh* const m_mesh;

		const bool m_sendNormals;
		const bool m_sendTextCoords;

        std::vector<Texture> m_textures;

        GLenum m_type;
        GLuint m_VAO;
        GLuint m_VBO_vertex;
        GLuint m_VBO_normal;
        GLuint m_EBO;
    };
}