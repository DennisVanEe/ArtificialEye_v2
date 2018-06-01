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

    glm::vec3 bc;
    baryCentric(point, m_mesh->getTransTriangle(triangle), &bc.x, &bc.y, &bc.z);
    // peform a linear interpolation of the normals:
    const MeshFace face = m_mesh->getFace(triangle);
    const glm::vec3 norm0 = m_mesh->getTransVertexNormal(face[0]);
    const glm::vec3 norm1 = m_mesh->getTransVertexNormal(face[1]);
    const glm::vec3 norm2 = m_mesh->getTransVertexNormal(face[2]);
    const glm::vec3 normal = glm::normalize(norm0 * bc.x + norm1 * bc.y + norm2 * bc.z);

    o_int->face = triangle;
    o_int->point = point;
    o_int->normal = normal;

    return true;
}

float ee::RTMesh::getRefraction() const
{
    return m_refraction;
}

float ee::RTMesh::getReflection() const
{
    return m_reflection;
}
