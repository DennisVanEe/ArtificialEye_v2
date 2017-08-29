#include "SkyBox.hpp"

const float ee::SkyBox::m_vertices[] = 
{
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};

ee::SkyBox::SkyBox(std::string textPack, std::string rootDir, std::vector<std::string> cubeFaces) :
    Drawable(textPack),
    m_cubeMap(loadCubeMap(rootDir, cubeFaces))
{
    GLuint VBO;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), &m_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
}

void ee::SkyBox::draw()
{
    glBindVertexArray(m_VAO);

    const Camera* camera = Renderer::getCamera();

    Drawable::m_shader->use();
    m_texturePack->setTexture(Drawable::m_shader, &m_shaderMaterial, camera); // sets whatever values it may want to set

    glm::mat4 lookAt = glm::mat4(glm::mat3(camera->viewMatrix()));
    glm::mat4 perspective = Renderer::getPerspective();
    glm::mat4 trans = perspective * lookAt;

    // all shaders need this:
    Drawable::m_shader->assignMat4f("u_posTrans", trans);
    Drawable::m_shader->assignMat4f("u_model", glm::mat4()); // in case this is needed

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}