#pragma once

#include "../Types.hpp"

#include <vector>
#include <string>
#include <memory>
#include <array>
#include <glm/matrix.hpp>

namespace ee
{
    class MeshFace
    {
    public:
        int& operator[](int index) 
		{ 
			return m_indices[index]; 
		}

        const int& operator[](int index) const 
		{
			return m_indices[index]; 
		}

        MeshFace() 
		{
		}

        MeshFace(int in0, int in1, int in2) :
			m_indices({ in0, in1, in2 })
		{
		}

    private:
        std::array<int, 3> m_indices;
    };
    //static_assert(sizeof(MeshFace) == 3 * sizeof(int) && alignof(MeshFace) == alignof(int), "MeshFace can't have any padding");

	using NormalBuffer = std::vector<glm::vec3>;
	using VertexBuffer = std::vector<glm::vec3>;
	using FaceBuffer = std::vector<MeshFace>;

	//
	// The format is designed to be easily converted to MFnMesh for interfacing with the Maya API
    class Mesh
    {
    public:
        Mesh() :
            m_updated(false)
        {
        }

		void create(VertexBuffer vertices, FaceBuffer faces, bool defaultNormals = true)
		{
			m_vertices = vertices;
			m_faces = faces;
			if (defaultNormals)
			{
				calcNormals();
			}
		}

		void create(VertexBuffer vertices, FaceBuffer faces, NormalBuffer normals)
		{
			m_vertices = vertices;
			m_faces = faces;
			m_normals = normals;
		}

        // Actually modifies the points
        void applyPointTrans(Mat4 mat);

		//
		// Modify underlying data directly:

		NormalBuffer& normalBuffer() 
		{
            m_updated = true;
			return m_normals;
		}

		const NormalBuffer& normalBuffer() const 
		{ 
            m_updated = true;
			return m_normals;
		}

		float* normalBufferRaw() 
		{
            m_updated = true;
			return reinterpret_cast<float*>(m_normals.data());
		}

		const float* normalBufferRaw() const
		{
            m_updated = true;
			return reinterpret_cast<const float*>(m_normals.data());
		}

		VertexBuffer& vertexBuffer()
		{
            m_updated = true;
			return m_vertices; 
		}

        const VertexBuffer& vertexBuffer() const
		{
            m_updated = true;
			return m_vertices; 
		}

		float* vertexBufferRaw()
		{
            m_updated = true;
			return reinterpret_cast<float*>(m_vertices.data());
		}

		const Float* vertexBufferRaw() const
		{
            m_updated = true;
			return reinterpret_cast<const Float*>(m_vertices.data()); 
		}

		FaceBuffer& faceBuffer() 
		{
            m_updated = true;
			return m_faces; 
		}

        const FaceBuffer& faceBuffer() const 
		{
            m_updated = true;
			return m_faces; 
		}

		int* faceBufferRaw() 
		{
            m_updated = true;
			return reinterpret_cast<int*>(m_faces.data());
		}

		const int* faceBufferRaw() const 
		{
            m_updated = true;
			return reinterpret_cast<const int*>(m_faces.data()); 
		}

		//
		// Access specific items:

        Vec3 getVertex(int vertexID) const { return m_vertices[vertexID]; }
        Vec3 getTransformedVertex(int vertexID)  const { return transPoint3(getModelTrans(), getVertex(vertexID)); }

		Vec3 getVertexNormal(int vertexID) const { return m_normals[vertexID]; }
		Vec3 getTransVertexNormal(int vertexID) const { return transVector3(getNormalModelTrans(), getVertexNormal(vertexID)); }

        MeshFace getFace(int meshFaceID) const { return m_faces[meshFaceID]; }

        Vec3 getFaceNormal(int meshFaceID) const;
		Vec3 getTransformedFaceNormal(int MeshFaceID) const { return transVector3(getNormalModelTrans(), getFaceNormal(MeshFaceID)); }

		int getNumVertices() const { return static_cast<int>(m_vertices.size()); }
		int getVertexID(int indexID) const { return faceBufferRaw()[indexID]; }
		int getNumIndices() const { return getNumFaces() * 3; }
		int getNumFaces() const { return static_cast<int>(m_faces.size()); }

        Float calcVolume() const;

		// due to winding order not being standardized,
		// normals will be calculated based on center position

        void calcNormals(); 

        void setModelTrans(Mat4 modelTrans) { m_modelTrans = modelTrans; m_normalModelTrans = glm::transpose(glm::inverse(modelTrans)); }
        Mat4 getModelTrans() const { return m_modelTrans; }
        Mat4 getNormalModelTrans() const { return m_normalModelTrans; }

        void updateVertex(const Vec3& vertex, std::size_t vertexID) { m_updated = true;  m_vertices[vertexID] = vertex; }
        void updateVertices(const VertexBuffer& vertices) { m_updated = true; m_vertices = vertices; }
        void updateMeshFaces(const FaceBuffer& faces) { m_updated = true; m_faces = faces; }

        bool wasUpdated() const
        {
            return m_updated;
        }

        void resetUpdated() const
        {
            m_updated = false;
        }

    private:
		// The meshes transformation (not visible through points unless specified)
        Mat4 m_modelTrans;
        Mat4 m_normalModelTrans;

        // Used with calculating normals
		VertexBuffer m_normals;
        VertexBuffer m_vertices;
        FaceBuffer	 m_faces;

        mutable bool m_updated;
    };
}