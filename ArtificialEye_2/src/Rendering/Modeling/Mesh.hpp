#pragma once

#include "../Drawable.hpp"
#include "../Renderer.hpp"
#include "../../Types.hpp"

#include <vector>
#include <string>
#include <memory>
#include <array>
#include <glad/glad.h>

namespace ee
{
    class MeshFace
    {
    public:
        int& operator()(int index) { return m_indices[index]; }
        const int& operator()(int index) const { return m_indices[index]; }

        MeshFace() {}
        MeshFace(int in0, int in1, int in2) : m_indices({ in0, in1, in2 }) {}

    private:
        std::array<int, 3> m_indices;
    };
    static_assert(sizeof(MeshFace) == 3 * sizeof(int), "MeshFace can't have any padding");

    template<typename T>
    struct TVertex
    {
        glm::tvec3<T> m_position;
        glm::tvec3<T> m_normal;
        glm::tvec2<T> m_textCoord;

        bool operator==(const TVertex& vert)
        {
            return m_position == vert.m_position &&
                m_normal == vert.m_normal && m_textCoord == vert.m_textCoord;
        }
        bool operator!=(const TVertex& vert)
        {
            return !((*this) == vert);
        }

        TVertex() {}
        TVertex(glm::tvec3<T> pos) : m_position(pos) {}
        TVertex(glm::tvec3<T> pos, glm::tvec3<T> textCoord, glm::tvec3<T> normal) : m_position(pos), m_textCoord(textCoord), m_normal(normal) {}
    };

    using Vertex = TVertex<Float>;
    using FloatVertex = TVertex<float>;

    // This is a check to make sure that certain meshes are made with the correct types.
    enum class MeshType {INDEXED_RECTANGLE, INDEXED_CUBE, ICOSPHERE, UVSPHERE, UNDEF};

    class Mesh
    {
    public:
        Mesh() : m_meshType(MeshType::UNDEF), m_updateCount(0) {}
        Mesh(std::vector<Vertex> vertices, std::vector<MeshFace> faces, MeshType type = MeshType::UNDEF) :
            m_vertices(std::move(vertices)), m_faces(std::move(faces)), m_updateCount(0),
            m_meshType(type) {}

        // Transforms the points
        void applyTransformation(Mat4 mat);

        std::vector<Vertex>& getVerticesData() { return m_vertices; }
        const std::vector<Vertex>& getVerticesData() const { return m_vertices; }

        std::vector<MeshFace>& getMeshFaceData() { return m_faces; }
        const std::vector<MeshFace>& getMeshFaceData() const { return m_faces; }

        virtual const Vertex& getVertex(int vertexID) const { return m_vertices[vertexID]; }
        const Vertex getTransformedVertex(int vertexID)  const
        {
            Vertex result;
            result.m_normal = transVector3(getNormalModelTrans(), getVertex(vertexID).m_normal);
            result.m_position = transPoint3(getModelTrans(), getVertex(vertexID).m_position);
            result.m_textCoord = getVertex(vertexID).m_textCoord;
            return result;
        }

        virtual std::size_t getNumVertices() const { return m_vertices.size(); }

        virtual std::size_t getVertexID(int indexID) const { return reinterpret_cast<const GLuint*>(m_faces.data())[indexID]; }
        virtual std::size_t getNumIndices() const { return m_faces.size() * 3; }

        virtual const MeshFace& getMeshFace(int meshFaceID) const { return m_faces[meshFaceID]; }
        virtual std::size_t getNumMeshFaces() const { return m_faces.size(); }

        virtual const glm::vec3 getNormal(int meshFaceID) const;

        virtual Float calcVolume() const;
        virtual void calcNormals(); // due to winding order not being standardized,
                                    // normals will be calculated based on center position

        void setModelTrans(Mat4 modelTrans) { m_modelTrans = modelTrans; m_normalModelTrans = glm::transpose(glm::inverse(modelTrans)); }
        Mat4 getModelTrans() const { return m_modelTrans; }
        Mat4 getNormalModelTrans() const { return m_normalModelTrans; }

        void updateVertex(const Vertex& vertex, std::size_t vertexID) { m_updateCount++; m_vertices[vertexID] = vertex; }
        void updateVertices(const std::vector<Vertex>& vertices) { m_updateCount++; m_vertices = vertices; }
        void updateMeshFaces(const std::vector<MeshFace>& faces) { m_updateCount++; m_faces = faces; }

        long long unsigned getUpdateCount() const { return m_updateCount; }

        MeshType getMeshType() const { return m_meshType; }
        void setMeshType(MeshType type) { m_meshType = type; }

    private:
        MeshType m_meshType;

        Mat4 m_modelTrans;
        Mat4 m_normalModelTrans;

        // Number of times the mesh had been updated
        long long unsigned m_updateCount;

    protected:
        // Used with calculating normals
        std::vector<Vec3> m_tempNormals;

        std::vector<Vertex> m_vertices;
        std::vector<MeshFace> m_faces;
    };
}