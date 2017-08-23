#include "Model.hpp"

ee::Model::Model(TexturePack* const textPack, const VertBuffer vertices, const IndexBuffer indices, GLenum type) :
    Drawable(textPack->getVShaderName(), textPack->getFShaderName()),
    m_type(type),
    m_texturePack(textPack->getCopy()),
    m_vertices(vertices),
    m_indices(indices),
    m_VAO(0)
{
    constructVAO();
}

ee::Model::Model(const Model& model) :
    Drawable(model),
    m_type(model.m_type),
    m_texturePack(model.m_texturePack->getCopy()),
    m_vertices(model.m_vertices),
    m_indices(model.m_indices),
    m_VAO(0)
{
    constructVAO();
}

ee::Model::Model(Model&& model) :
    Drawable(model),
    m_type(model.m_type),
    m_texturePack(std::move(model.m_texturePack)),
    m_vertices(std::move(model.m_vertices)),
    m_indices(std::move(model.m_indices)),
    m_VAO(model.m_VAO)
{
}

ee::Model::~Model()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VBO);
}

void ee::Model::draw()
{
    Drawable::m_shader->use();
    m_texturePack->setTexture(Drawable::m_shader); // sets the appropriate color

    const Camera* camera = Renderer::getCamera();
    glm::mat4 lookAt = camera->viewMatrix();
    glm::mat4 perspective = Renderer::getPerspective();

    glm::mat4 trans = perspective * lookAt * m_modelTrans;

    Drawable::m_shader->assignMat4f("u_posTrans", trans);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ee::Model::constructVAO()
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