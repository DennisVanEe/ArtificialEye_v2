#include "DynamicMesh.hpp"

ee::DynamicMesh::DynamicMesh(std::string textPack, VertBuffer vertices, IndexBuffer indices) :
    Mesh(textPack, vertices, indices, GL_DYNAMIC_DRAW)
{
}

ee::DynamicMesh::DynamicMesh(const DynamicMesh& model) :
    Mesh(model)
{
}

ee::DynamicMesh::DynamicMesh(DynamicMesh&& mesh) :
    Mesh(std::move(mesh))
{
}

void ee::DynamicMesh::setVertex(const Vertex& vertex, std::size_t vertexID)
{
    m_vertices[vertexID] = vertex; // update it here
                                   // and update it in the buffer itself
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_position), sizeof(Vector3), glm::value_ptr(vertex.m_position));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_normal), sizeof(Vector3), glm::value_ptr(vertex.m_normal));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_textCoord), sizeof(Vector3), glm::value_ptr(vertex.m_textCoord));
}