#include "RTObjectMesh.hpp"

#include "RTUtility.hpp"

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