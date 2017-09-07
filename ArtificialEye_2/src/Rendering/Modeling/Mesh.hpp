#pragma once

#include "../Drawable.hpp"
#include "../Renderer.hpp"
#include "../TexturePacks/TexturePack.hpp"
#include "../../Types.hpp"

#include <vector>
#include <string>
#include <memory>
#include <glad/glad.h>

namespace ee
{
    struct MeshFace
    {
        GLuint m_indices[3];
    };
    static_assert(sizeof(MeshFace) == 3 * sizeof(GLuint), "MeshFace can't have anny padding (t0 maintain per vertex access");

    struct Vertex
    {
        Vector3 m_position;
        Vector3 m_normal;
        Vector3 m_textCoord;

        bool operator==(const Vertex& vert)
        {
            return m_position == vert.m_position && m_normal == vert.m_normal && m_textCoord == vert.m_textCoord;
        }

        Vertex() {}
        Vertex(Vector3 pos) : m_position(pos) {}
        Vertex(Vector3 pos, Vector3 textCoord, Vector3 normal) : m_position(pos), m_textCoord(textCoord), m_normal(normal) {}
    };

    using VertBuffer = std::vector<Vertex>;
    using MeshFaceBuffer = std::vector<MeshFace>;

    class Mesh : public Drawable
    {
    public:
        explicit Mesh(std::string textPack, VertBuffer vertices, MeshFaceBuffer faces, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);
        explicit Mesh(const Mesh& model);
        Mesh(Mesh&& model);

        virtual ~Mesh();

        void applyTransformation(glm::mat4 mat); // this actually changes the individual vertex information

        virtual const Vertex& getVertex(std::size_t vertexID) const { return m_vertices[vertexID]; }
        virtual std::size_t getNumVertices() const { return m_vertices.size(); }

        virtual std::size_t getVertexID(std::size_t indexID) const { return reinterpret_cast<const GLuint*>(m_faces.data())[indexID]; }
        virtual std::size_t getNumIndices() const { return m_faces.size() * (sizeof(MeshFace) / sizeof(GLuint)); }

        virtual const MeshFace& getMeshFace(std::size_t meshFaceID) const { return m_faces[meshFaceID]; }
        virtual std::size_t getNumMeshFaces() const { return m_faces.size(); }

        virtual const Vector3 getNormal(std::size_t meshFaceID) const
        {
            const auto& f = m_faces[meshFaceID];

            Vector3 v0 = m_vertices[f.m_indices[0]].m_position;
            Vector3 v1 = m_vertices[f.m_indices[1]].m_position;
            Vector3 v2 = m_vertices[f.m_indices[2]].m_position;

            Vector3 e0 = v1 - v0;
            Vector3 e1 = v2 - v0;
            return glm::normalize(glm::cross(e0, e1));
        }

        // used to position the model in the world
        glm::mat4 m_modelTrans;

        virtual void draw() override;

        virtual float calcVolume() const;
        virtual void calcNormals();

    private:
        void constructVAO();
        const GLenum m_type; // for when copying the object

    protected:
        std::vector<Vector3> m_tempNormals;

        VertBuffer m_vertices;
        MeshFaceBuffer m_faces;

        // if a subclass wants to take care of the models
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_EBO;
    };
}