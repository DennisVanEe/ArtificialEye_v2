#include "RTObjectMesh.hpp"
#include "RTUtility.hpp"

ee::RTObjectMesh::RTObjectMesh(std::string name, Mesh* mesh, float refraction, bool reflective) :
    RTObject(name, refraction, reflective),
    m_intersected(false),
    m_mesh(mesh)
{
}

glm::vec3 ee::RTObjectMesh::intPoint() const
{
    return m_intersected ? m_cachedPoint : glm::vec3(NaN);
}

glm::vec3 ee::RTObjectMesh::intNormalFace() const
{
    return m_intersected ? m_mesh->getFaceNormal(m_cachedFace) : glm::vec3(NaN);
}

int ee::RTObjectMesh::intFace() const
{
    return m_intersected ? m_cachedFace : -1;
}

glm::mat4 ee::RTObjectMesh::getPosition() const
{
    return m_mesh->getModelTrans();
}

ee::RTObject* ee::RTObjectMesh::getCopy() const
{
    return new RTObjectMesh(*this);
}

const ee::Mesh* ee::RTObjectMesh::getMesh() const
{
    return m_mesh;
}

bool ee::RTObjectMesh::calcIntersection(Ray ray, int ignoreTriangle) const
{
	int minFace = -1;
	glm::vec3 minPoint = glm::vec3(0.f, 0.f, 0.f); // the point where the intersection would occur
	float minDist = std::numeric_limits<float>::infinity();

	for (int i = 0; i < m_mesh->getNumFaces(); i++)
	{
		if (i != ignoreTriangle)
		{
			const MeshFace& face = m_mesh->getFace(i);

			// transform those points:
			const glm::vec3 p0 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[0]));
			const glm::vec3 p1 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[1]));
			const glm::vec3 p2 = transPoint3(m_mesh->getModelTrans(), m_mesh->getVertex(face[2]));

			glm::vec3 intpoint;
			const bool result = intersectTriangle(ray, p0, p1, p2, &intpoint);
			if (result) // there was an intersection
			{
				const glm::vec3 diff = intpoint - ray.origin;
				const float len = glm::dot(diff, diff); // find the length
				if (len < minDist)
				{
					minDist = len;
					minFace = i;
					minPoint = intpoint;
				}
			}
		}
	}

	m_cachedFace = minFace;
	m_cachedPoint = minPoint;

	m_intersected = minFace != -1;
	return m_intersected;
}

glm::vec3 ee::RTObjectMesh::intNormalInterpolated() const
{
	return getNormal(m_mesh, m_cachedFace, m_cachedPoint);
}