#include "DynamicMesh.hpp"

void ee::DynamicMesh::recalcNormals()
{
    m_tempNormals.clear();
    m_tempNormals.resize(m_vertices.size());

    for (MeshFace& face : m_indices)
    {
        Vector3 v0 = m_vertices[face.m_indices[0]].m_position;
        Vector3 v1 = m_vertices[face.m_indices[1]].m_position;
        Vector3 v2 = m_vertices[face.m_indices[2]].m_position;

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;
        Vector3 tempNormal = glm::normalize(glm::cross(e0, e1));

        m_tempNormals[face.m_indices[0]] += tempNormal;
        m_tempNormals[face.m_indices[1]] += tempNormal;
        m_tempNormals[face.m_indices[2]] += tempNormal;
    }

    // normalize those results and update the model itself:
    for (std::size_t i = 0; i < m_tempNormals.size(); i++)
    {
        Vertex vert = getVertex(i);
        vert.m_normal = glm::normalize(m_tempNormals[i]);
        setVertex(vert, i);
    }
}

void ee::DynamicMesh::setVertex(const Vertex& vertex, std::size_t vertexID)
{
    m_vertices[vertexID] = vertex; // update it here
                                   // and update it in the buffer itself
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_position), sizeof(Vector3), glm::value_ptr(vertex.m_position));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_normal), sizeof(Vector3), glm::value_ptr(vertex.m_normal));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_textCoord), sizeof(Vector3), glm::value_ptr(vertex.m_textCoord));
}