#include "RTObject.hpp"

namespace ee
{
	class RTObjectMesh : public RTObject
	{
	public:
		RTObjectMesh(std::string name, Mesh* mesh, float refraction = std::numeric_limits<float>::quiet_NaN(), bool reflective = false) :
			RTObject(name, refraction, reflective), 
			m_intersected(false), 
			m_mesh(mesh) 
		{
		}

		bool calcIntersection(Ray ray, int ignoreTriangle) const override;
		glm::vec3 intNormalInterpolated() const override;

        glm::vec3 intPoint() const override
		{
			float nan = std::numeric_limits<float>::quiet_NaN();
			return m_intersected ? m_cachedPoint : glm::vec3(nan, nan, nan); 
		}

        glm::vec3 intNormalFace() const override
		{
			float nan = std::numeric_limits<float>::quiet_NaN();
			return m_intersected ? m_mesh->getFaceNormal(m_cachedFace) : glm::vec3(nan, nan, nan); ;
		}

		int intFace() const override 
		{ 
			return m_cachedFace; 
		}

		glm::mat4 getPosition() const override 
		{ 
			return m_mesh->getModelTrans(); 
		}

		void setPosition(glm::mat4 pos) override 
		{ 
			m_mesh->setModelTrans(pos); 
		}

		RTObject* getCopy() const override 
		{ 
			return new RTObjectMesh(*this); 
		}

		Mesh* getMesh()
		{
			return m_mesh;
		}

		const Mesh* getMesh() const
		{
			return m_mesh;
		}

		//UDP getUDP() const override
		//{
		//	const float* rawdata = m_mesh->vertexBufferRaw();
		//	const int rawdataSize = m_mesh->getNumVertices() * 3;
		//	const int* rawindices = m_mesh->faceBufferRaw();
		//	const int rawindicesSize = m_mesh->getNumIndices();

		//	return UDP(getName().c_str(), UDP_TYPE_MESH, m_mesh->getModelTrans(), rawdata, rawdataSize, rawindices, rawindicesSize);
		//}

	private:
		mutable bool m_intersected;
		mutable glm::vec3 m_cachedPoint;
		mutable int m_cachedFace;

		Mesh* m_mesh;
	};
}