#include "Mesh.hpp"

ee::Mesh::Mesh(const std::string textPack, const VertBuffer vertices, const IndexBuffer indices, GLenum type) :
    Drawable(textPack),
    m_type(type),
    m_vertices(vertices),
    m_indices(indices),
    m_VAO(0)
{
    constructVAO();
}

const ee::Vertex& ee::Mesh::getVertex(std::size_t vertexID) const
{
    return m_vertices[vertexID];
}

std::size_t ee::Mesh::getNumVertices() const
{
    return m_vertices.size();
}

std::size_t ee::Mesh::getVertexID(std::size_t indexID) const
{
    return m_indices[indexID];
}

std::size_t ee::Mesh::getNumIndices() const
{
    return m_indices.size();
}

ee::Mesh::Mesh(const Mesh& model) :
    Drawable(model),
    m_type(model.m_type),
    m_vertices(model.m_vertices),
    m_indices(model.m_indices),
    m_VAO(0)
{
    constructVAO();
}

ee::Mesh::Mesh(Mesh&& mesh) :
    Drawable(mesh),
    m_type(mesh.m_type),
    m_vertices(std::move(mesh.m_vertices)),
    m_indices(std::move(mesh.m_indices)),
    m_VAO(mesh.m_VAO)
{
    mesh.m_VAO = 0;
}

ee::Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
}

void ee::Mesh::draw()
{
    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();
    m_texturePack->setTexture(Drawable::m_shader, &m_shaderMaterial, camera); // sets whatever values it may want to set

    glm::mat4 lookAt = camera->viewMatrix();
    glm::mat4 perspective = Renderer::getPerspective();
    glm::mat4 trans = perspective * lookAt * m_modelTrans;

    // all shaders need this:
    Drawable::m_shader->assignMat4f("u_posTrans", trans);
    Drawable::m_shader->assignMat4f("u_model", m_modelTrans); // in case this is needed

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

ee::Float ee::Mesh::calcVolume() const
{
    Float total = F(0);
    for (std::size_t i = 0; i < m_indices.size();)
    {
        // get triangle indices:
        GLuint i0 = m_indices[i++];
        GLuint i1 = m_indices[i++];
        GLuint i2 = m_indices[i++];

        Float stv = glm::dot(m_vertices[i0].m_position,
            glm::cross(m_vertices[i1].m_position, m_vertices[i2].m_position)) / F(6);
        total += stv;
    }

    return std::abs(total);
}

void ee::Mesh::constructVAO()
{
    if (m_VAO != 0)
    {
        return;
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), m_type);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), m_type);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textCoord));

    glBindVertexArray(0);
}

void ee::Mesh::calcNormals()
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
        m_vertices[i] = vert;
    }
}