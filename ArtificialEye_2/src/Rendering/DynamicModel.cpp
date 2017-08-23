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

const ee::Vertex& ee::DynamicModel::getVertex(std::size_t vertexID) const
{
    return m_vertices[vertexID];
}

void ee::DynamicModel::setVertex(const Vertex& vertex, std::size_t vertexID)
{
    m_vertices[vertexID] = vertex; // update it here
                                   // and update it in the buffer itself
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_position), sizeof(Vector3), glm::value_ptr(vertex.m_position));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_normal), sizeof(Vector3), glm::value_ptr(vertex.m_normal));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexID + offsetof(Vertex, m_textCoord), sizeof(Vector3), glm::value_ptr(vertex.m_textCoord));
}

std::size_t ee::DynamicModel::getNumVertices() const
{
    return m_vertices.size();
}

std::size_t ee::DynamicModel::getVertexID(std::size_t indexID) const
{
    return m_indices[indexID];
}

std::size_t ee::DynamicModel::getNumIndices() const
{
    return m_indices.size();
}