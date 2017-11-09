#include "Mesh.hpp"

void ee::Mesh::applyTransformation(Mat4 mat)
{
    Mat4 normMat = glm::transpose(glm::inverse(mat));
    for (auto& vec : m_vertices)
    {
        vec.m_position = transPoint3(mat, vec.m_position);
        vec.m_normal = transVector3(mat, vec.m_normal);
    }
}

const glm::vec3 ee::Mesh::getNormal(int meshFaceID) const
{
    const auto& f = m_faces[meshFaceID];

    Vec3 v0 = m_vertices[f(0)].m_position;
    Vec3 v1 = m_vertices[f(1)].m_position;
    Vec3 v2 = m_vertices[f(2)].m_position;

    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;
    return glm::normalize(glm::cross(e0, e1));
}

ee::Float ee::Mesh::calcVolume() const
{
    Float total = 0.f;
    for (const auto& f : m_faces)
    {
        Float stv = glm::dot(m_vertices[f(0)].m_position,
            glm::cross(m_vertices[f(1)].m_position, m_vertices[f(2)].m_position)) / 6.0;
        total += stv;
    }

    return std::abs(total);
}

void ee::Mesh::calcNormals()
{
    m_tempNormals.clear();
    m_tempNormals.resize(m_vertices.size());

    for (const auto& f : m_faces)
    {
        const Vec3 v0 = m_vertices[f(0)].m_position;
        const Vec3 v1 = m_vertices[f(1)].m_position;
        const Vec3 v2 = m_vertices[f(2)].m_position;

        const Vec3 e0 = v1 - v0;
        const Vec3 e1 = v2 - v0;
        const Vec3 tempNormal = flipSameDir(glm::normalize(glm::cross(e0, e1)), glm::normalize(v0 + v1 + v2)); // calculate the temp normal

        m_tempNormals[f(0)] += tempNormal;
        m_tempNormals[f(1)] += tempNormal;
        m_tempNormals[f(2)] += tempNormal;
    }

    // normalize those results and update the model itself:
    for (int i = 0; i < m_tempNormals.size(); i++)
    {
        Vertex vert = getVertex(i);
        vert.m_normal = glm::normalize(m_tempNormals[i]);
        m_vertices[i] = vert;
    }
}