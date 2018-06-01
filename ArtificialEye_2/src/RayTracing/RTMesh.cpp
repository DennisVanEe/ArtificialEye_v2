#include "RTMesh.hpp"
#include "RTUtility.hpp"

ee::RTMesh::RTMesh(const Mesh* mesh, float refraction, bool reflection) :
    m_tree(mesh),
    m_refraction(refraction),
    m_reflection(reflection),
    m_mesh(mesh)
{
}

bool ee::RTMesh::intersect(Ray ray, int ignoreFace, RTMeshIntersection* o_int) const
{
    glm::vec3 point;
    int triangle;
    if (!m_tree.intersect(0, ray, ignoreFace, &point, &triangle))
    {
        return false;
    }

    o_int->face = triangle;
    o_int->point = point;

    glm::vec3 bc;
    baryCentric(point, m_mesh->getTransTriangle(triangle), &bc.x, &bc.y, &bc.z);
    // peform a linear interpolation of the normals:

}

bool ee::RTObjectMesh::calcIntersection(Ray ray, int ignoreTriangle, bool towardsPhoto) const
{
	int minFace = -1;
	glm::vec3 minPoint = glm::vec3(0.f, 0.f, 0.f); // the point where the intersection would occur
	float minDist = std::numeric_limits<float>::infinity();

    const int startPosition = towardsPhoto ? (m_mesh->getNumFaces() / 2.f + 0.5f) : 0;
    const int endPosition = towardsPhoto ? m_mesh->getNumFaces() : (m_mesh->getNumFaces() / 2.f + 0.5f);

	for (int i = startPosition; i < endPosition; i++)
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