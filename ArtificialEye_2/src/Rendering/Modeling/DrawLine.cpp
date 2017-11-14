#include "DrawLine.hpp"

#include "../Renderer.hpp"

ee::DrawLine::DrawLine(const std::string& textPack, glm::vec3 p0, glm::vec3 p1, int priority, GLenum dataUsage) :
    Drawable(textPack, priority),
    m_type(dataUsage),
    m_VAO(0)
{
    m_points[0] = p0;
    m_points[1] = p1;

    constructVAO();
}

ee::DrawLine::DrawLine(const DrawLine& other) :
    Drawable(other),
    m_type(other.m_type)
{
    m_points[0] = other.m_points[0];
    m_points[1] = other.m_points[1];

    constructVAO();
}

ee::DrawLine::DrawLine(DrawLine&& other) :
    Drawable(std::move(other)),
    m_type(other.m_type),
    m_VAO(other.m_VAO),
    m_VBO(other.m_VBO)
{
    m_points[0] = other.m_points[0];
    m_points[1] = other.m_points[1];

    other.m_VAO = 0;
    other.m_VBO = 0;
}

ee::DrawLine::~DrawLine()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void ee::DrawLine::setPoint0(glm::vec3 p0)
{
    m_points[0] = p0;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(p0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::DrawLine::setPoint1(glm::vec3 p1)
{
    m_points[1] = p1;
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3), sizeof(glm::vec3), glm::value_ptr(p1));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ee::DrawLine::setRay(Ray ray, float length)
{
    setPoint0(ray.m_origin);
    glm::vec3 p1 = glm::vec3(ray.m_dir) * length + glm::vec3(ray.m_origin);
    setPoint1(p1);
}

void ee::DrawLine::setLine(Line line)
{
    setPoint0(line.m_start);
    setPoint1(line.m_end);
}

glm::vec3 ee::DrawLine::getPoint0() const
{ 
    return m_points[0]; 
}

glm::vec3 ee::DrawLine::getPoint1() const
{ 
    return m_points[1]; 
}

ee::Ray ee::DrawLine::getRay() const
{
    Ray result;
    result.m_dir = glm::normalize(m_points[1] - m_points[0]);
    result.m_origin = m_points[0];
    return result;
}

void ee::DrawLine::draw()
{
    glBindVertexArray(m_VAO);

    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();
    m_texturePack->preDraw(Drawable::m_shader, &m_shaderMaterial, camera); // sets whatever values it may want to set

    glm::mat4 lookAt = camera->viewMatrix();
    glm::mat4 perspective = Renderer::getPerspective();
    glm::mat4 trans = perspective * lookAt;

    // all shaders need this:
    Drawable::m_shader->assignMat4("u_posTrans", trans);
    Drawable::m_shader->assignMat4("u_model", glm::mat4()); // in case this is needed

    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);

    m_texturePack->postDraw();
}

void ee::DrawLine::constructVAO()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, m_points, m_type);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}