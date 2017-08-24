#include "DynamicModel.hpp"

ee::DynamicModel::DynamicModel(TexturePack* const textPack, VertBuffer vertices, IndexBuffer indices) :
    Model(textPack, vertices, indices, GL_DYNAMIC_DRAW)
{
}

ee::DynamicModel::DynamicModel(const DynamicModel& model) :
    Model(model)
{
}

ee::DynamicModel::DynamicModel(DynamicModel&& model) :
    Model(std::move(model))
{
}

void ee::DynamicModel::setVertex(const Vertex& vertex, std::size_t vertexID)
{
    m_vertices[vertexID] = vertex; // update it here
                                   // and update it in the buffer itself
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_position), sizeof(Vector3), glm::value_ptr(vertex.m_position));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_normal), sizeof(Vector3), glm::value_ptr(vertex.m_normal));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_textCoord), sizeof(Vector3), glm::value_ptr(vertex.m_textCoord));
}