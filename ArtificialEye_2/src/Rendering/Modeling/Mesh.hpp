#pragma once

#include "../Drawable.hpp"
#include "../Renderer.hpp"
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
        glm::vec3 m_position;
        glm::vec3 m_normal;
        glm::vec3 m_textCoord;

        bool operator==(const Vertex& vert)
        {
            return m_position == vert.m_position && m_normal == vert.m_normal && m_textCoord == vert.m_textCoord;
        }

        Vertex() {}
        Vertex(glm::vec3 pos) : m_position(pos) {}
        Vertex(glm::vec3 pos, glm::vec3 textCoord, glm::vec3 normal) : m_position(pos), m_textCoord(textCoord), m_normal(normal) {}
    };

    using VertBuffer     = std::vector<Vertex>;
    using MeshFaceBuffer = std::vector<MeshFace>;

    class Mesh : public Drawable
    {
    public:
        Mesh(std::string textPack, VertBuffer vertices, MeshFaceBuffer faces, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);
        Mesh(const Mesh& other);
        Mesh(Mesh&& other);
        virtual ~Mesh();

        // this actually changes the individual vertex information
        void applyTransformation(glm::mat4 mat);

        VertBuffer& getVerticesData();
        const VertBuffer& getVerticesData() const;

        MeshFaceBuffer& getMeshFaceData();
        const MeshFaceBuffer& getMeshFaceData() const;

        virtual const Vertex& getVertex(std::size_t vertexID) const;
        virtual std::size_t getNumVertices() const;

        virtual std::size_t getVertexID(std::size_t indexID) const;
        virtual std::size_t getNumIndices() const;

        virtual const MeshFace& getMeshFace(std::size_t meshFaceID) const;
        virtual std::size_t getNumMeshFaces() const;

        virtual const glm::vec3 getNormal(std::size_t meshFaceID) const;

        // used to position the model in the world
        virtual void draw() override;

        virtual float calcVolume() const;
        virtual void calcNormals();

        void setModelTrans(glm::mat4 modelTrans);
        glm::mat4 getModelTrans() const;
        glm::mat4 getNormalModelTrans() const;

    private:
        void constructVAO();
        const GLenum m_type; // for when copying the object

        glm::mat4 m_modelTrans;
        glm::mat4 m_normalModelTrans;

    protected:
        std::vector<glm::vec3> m_tempNormals;

        VertBuffer     m_vertices;
        MeshFaceBuffer m_faces;

        // if a subclass wants to take care of the models
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_EBO;
    };
}