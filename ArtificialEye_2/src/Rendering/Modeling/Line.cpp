#include "Line.hpp"

ee::Line::Line(std::string textPack, glm::vec3 p0, glm::vec3 p1, int priority, GLenum dataUsage) :
    Drawable(textPack, priority),
    m_VAO(0)
{
    m_points[0] = p0;
    m_points[1] = p1;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, m_points, dataUsage);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::setPoint0(glm::vec3 p0)
{
    m_points[0] = p0;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(p0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::setPoint1(glm::vec3 p1)
{
    m_points[1] = p1;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(p1));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::Line::setRay(Ray ray, float length)
{
    setPoint0(ray.m_origin);
    glm::vec3 p1 = ray.m_dir * length + ray.m_origin;
    setPoint1(p1);
}

void ee::Line::setLine(LineSegment line)
{
    setPoint0(line.m_start);
    setPoint1(line.m_end);
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