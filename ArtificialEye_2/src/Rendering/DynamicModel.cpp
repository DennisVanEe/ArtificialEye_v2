#include "DynamicModel.hpp"

ee::DynamicModel::DynamicModel(std::string textPack, VertBuffer vertices, IndexBuffer indices) :
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

void ee::DynamicModel::recalcNormals()
{
    m_tempNormals.clear();
    m_tempNormals.resize(m_vertices.size());

    for (std::size_t i = 0; i < m_indices.size();)
    {
        GLuint i0 = m_indices[i++];
        GLuint i1 = m_indices[i++];
        GLuint i2 = m_indices[i++];

        Vector3 v0 = m_vertices[i0].m_position;
        Vector3 v1 = m_vertices[i1].m_position;
        Vector3 v2 = m_vertices[i2].m_position;

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;
        Vector3 tempNormal = glm::normalize(glm::cross(e0, e1));

        m_tempNormals[i0] += tempNormal;
        m_tempNormals[i1] += tempNormal;
        m_tempNormals[i2] += tempNormal;
    }

    // normalize those results and update the model itself:
    for (std::size_t i = 0; i < m_tempNormals.size(); i++)
    {
        Vertex vert = getVertex(i);
        vert.m_normal = glm::normalize(m_tempNormals[i]);
        setVertex(vert, i);
    }
}