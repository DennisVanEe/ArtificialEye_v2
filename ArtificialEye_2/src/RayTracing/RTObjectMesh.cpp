#include "RTObjectMesh.hpp"
#include "RTUtility.hpp"

bool ee::RTObjectMesh::calcIntersection(Ray ray, int ignoreTriangle) const
{
	int minFace = -1;
	glm::vec3 minPoint = Vec3(0.f, 0.f, 0.f); // the point where the intersection would occur
	float minDist = std::numeric_limits<float>::max();

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
				const float len = glm::length(diff); // find the length
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

	return minFace != -1;
}

glm::vec3 ee::RTObjectMesh::intNormalInterpolated() const
{
	MeshFace face = m_mesh->getFace(m_cachedFace);
    glm::vec3 vert0 = m_mesh->getTransformedVertex(face[0]);
    glm::vec3 vert1 = m_mesh->getTransformedVertex(face[1]);
    glm::vec3 vert2 = m_mesh->getTransformedVertex(face[2]);

    glm::vec3 norm0 = m_mesh->getTransformedFaceNormal(face[0]);
    glm::vec3 norm1 = m_mesh->getTransformedFaceNormal(face[1]);
    glm::vec3 norm2 = m_mesh->getTransformedFaceNormal(face[2]);

	float u, v, w;
	baryCentric(m_cachedPoint, vert0, vert1, vert2, &u, &v, &w);
	glm::vec3 normal = glm::normalize(norm0 * u + norm1 * v + norm2 * w);
	return normal;
}