#include "Mesh.hpp"

ee::Mesh::Mesh(const std::string textPack, const VertBuffer vertices, const MeshFaceBuffer faces, int priority, GLenum type) :
    Drawable(textPack, priority),
    m_type(type),
    m_vertices(vertices),
    m_faces(faces),
    m_VAO(0)
{
    constructVAO();
}

ee::Mesh::Mesh(const Mesh& model) :
    Drawable(model),
    m_type(model.m_type),
    m_vertices(model.m_vertices),
    m_faces(model.m_faces),
    m_VAO(0)
{
    constructVAO();
}

ee::Mesh::Mesh(Mesh&& mesh) :
    Drawable(mesh),
    m_type(mesh.m_type),
    m_vertices(std::move(mesh.m_vertices)),
    m_faces(std::move(mesh.m_faces)),
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

void ee::Mesh::applyTransformation(glm::mat4 mat)
{
    glm::mat4 normMat = glm::transpose(glm::inverse(mat));
    for (auto& vec : m_vertices)
    {
        vec.m_position = Vector3(mat * glm::vec4(vec.m_position, 1.f));
        vec.m_normal = Vector3(normMat * glm::vec4(vec.m_normal, 0.f));
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
}

void ee::Mesh::draw()
{
    glBindVertexArray(m_VAO);

    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();
    m_texturePack->preDraw(Drawable::m_shader, &m_shaderMaterial, camera); // sets whatever values it may want to set

    glm::mat4 lookAt = camera->viewMatrix();
    glm::mat4 perspective = Renderer::getPerspective();
    glm::mat4 trans = perspective * lookAt * m_modelTrans;

    // all shaders need this:
    Drawable::m_shader->assignMat4f("u_posTrans", trans);
    Drawable::m_shader->assignMat4f("u_model", m_modelTrans); // in case this is needed

    glDrawElements(GL_TRIANGLES, getNumIndices(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_texturePack->postDraw();
}

float ee::Mesh::calcVolume() const
{
    float total = 0.f;
    for (const auto& f : m_faces)
    {
        float stv = glm::dot(m_vertices[f.m_indices[0]].m_position,
            glm::cross(m_vertices[f.m_indices[1]].m_position, m_vertices[f.m_indices[2]].m_position)) / 6.f;
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshFace) * m_faces.size(), m_faces.data(), m_type);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_textCoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ee::Mesh::calcNormals()
{
    m_tempNormals.clear();
    m_tempNormals.resize(m_vertices.size());

    for (const auto& f : m_faces)
    {
        Vector3 v0 = m_vertices[f.m_indices[0]].m_position;
        Vector3 v1 = m_vertices[f.m_indices[1]].m_position;
        Vector3 v2 = m_vertices[f.m_indices[2]].m_position;

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;
        Vector3 tempNormal = glm::normalize(glm::cross(e0, e1));

        m_tempNormals[f.m_indices[0]] += tempNormal;
        m_tempNormals[f.m_indices[1]] += tempNormal;
        m_tempNormals[f.m_indices[2]] += tempNormal;
    }

    // normalize those results and update the model itself:
    for (std::size_t i = 0; i < m_tempNormals.size(); i++)
    {
        Vertex vert = getVertex(i);
        vert.m_normal = glm::normalize(m_tempNormals[i]);
        m_vertices[i] = vert;
    }
}