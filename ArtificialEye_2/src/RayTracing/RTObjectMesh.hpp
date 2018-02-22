#pragma once

#include "RTObject.hpp"

namespace ee
{
	class RTObjectMesh : public RTObject
	{
	public:
		inline RTObjectMesh(std::string name, Mesh* mesh, float refraction = std::numeric_limits<float>::quiet_NaN(), bool reflective = false);

		bool calcIntersection(Ray ray, int ignoreTriangle) const override;
		glm::vec3 intNormalInterpolated() const override;

		inline glm::vec3 intPoint() const override;
		inline glm::vec3 intNormalFace() const override;
		inline int intFace() const override;

		inline glm::mat4 getPosition() const override;
		inline void setPosition(glm::mat4 pos) override;

		inline Mesh* getMesh();
		inline const Mesh* getMesh() const;

		inline RTObject* getCopy() const override;

	private:
		mutable bool		m_intersected;
		mutable glm::vec3	m_cachedPoint;
		mutable int			m_cachedFace;

		Mesh* m_mesh;
	};
}

ee::RTObjectMesh::RTObjectMesh(std::string name, Mesh* mesh, float refraction, bool reflective) :
	RTObject(name, refraction, reflective),
	m_intersected(false),
	m_mesh(mesh)
{
}

glm::vec3 ee::RTObjectMesh::intPoint() const
{
	float nan = std::numeric_limits<float>::quiet_NaN();
	return m_intersected ? m_cachedPoint : glm::vec3(nan, nan, nan);
}

glm::vec3 ee::RTObjectMesh::intNormalFace() const
{
	float nan = std::numeric_limits<float>::quiet_NaN();
	return m_intersected ? m_mesh->getFaceNormal(m_cachedFace) : glm::vec3(nan, nan, nan); ;
}

int ee::RTObjectMesh::intFace() const
{
	return m_cachedFace;
}

glm::mat4 ee::RTObjectMesh::getPosition() const
{
	return m_mesh->getModelTrans();
}

void ee::RTObjectMesh::setPosition(glm::mat4 pos)
{
	m_mesh->setModelTrans(pos);
}

ee::RTObject* ee::RTObjectMesh::getCopy() const
{
	return new RTObjectMesh(*this);
}

ee::Mesh* ee::RTObjectMesh::getMesh()
{
	return m_mesh;
}

const ee::Mesh* ee::RTObjectMesh::getMesh() const
{
	return m_mesh;
}