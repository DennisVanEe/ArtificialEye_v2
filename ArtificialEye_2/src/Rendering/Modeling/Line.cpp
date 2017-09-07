#include "Line.hpp"

ee::Line::Line(std::string textPack, Vector3 p0, Vector3 p1, int priority, GLenum dataUsage) :
    Drawable(textPack, priority),
    m_VAO(0)
{
    m_points[0] = p0;
    m_points[1] = p1;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * 2, m_points, dataUsage);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::setPoint0(Vector3 p0)
{
    m_points[0] = p0;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3), glm::value_ptr(p0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::setPoint1(Vector3 p1)
{
    m_points[1] = p1;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3), sizeof(Vector3), glm::value_ptr(p1));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::draw()
{
    glBindVertexArray(m_VAO);

    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();
    m_texturePack->preDraw(Drawable::m_shader, &m_shaderMaterial, camera); // sets whatever values it may want to set

    glm::mat4 lookAt = camera->viewMatrix();
    glm::mat4 perspective = Renderer::getPerspective();
    glm::mat4 trans = perspective * lookAt;

    // all shaders need this:
    Drawable::m_shader->assignMat4f("u_posTrans", trans);
    Drawable::m_shader->assignMat4f("u_model", glm::mat4()); // in case this is needed

    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);

    m_texturePack->postDraw();
}