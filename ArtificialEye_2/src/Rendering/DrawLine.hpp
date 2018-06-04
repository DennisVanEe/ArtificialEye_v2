#pragma once

#include "Drawable.hpp"
#include "../Types.hpp"

#include <glad/glad.h>

namespace ee
{
    //
    // DrawLine: Class for drawing lines in OpenGL
    //
    class DrawLine : public Drawable
    {
    public:
        DrawLine(const std::string& textPack, glm::vec3 p0, glm::vec3 p1, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);
        DrawLine(const std::string& textPack, Line line, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);
        DrawLine(const DrawLine& other);
        DrawLine(DrawLine&& other);
        ~DrawLine();

        void setPoint0(glm::vec3 p0);
        void setPoint1(glm::vec3 p1);

        void setRay(Ray ray, float length);
        void setLine(Line line);

        glm::vec3 getPoint0() const;
        glm::vec3 getPoint1() const;

        Ray getRay() const;

        void draw() override;

    private:
        void constructVAO();
        const GLenum m_type;

        glm::vec3 m_points[2];

        GLuint m_VAO;
        GLuint m_VBO;
    };

    class DrawPath : public Drawable
    {
    public:
        __forceinline DrawPath(const std::string& textPack, Path path, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW) :
            Drawable(textPack, priority),
            m_type(dataUsage),
            m_path(path),
            m_VAO(0)
        {
            constructVAO();
        }

        __forceinline DrawPath(const DrawPath& other) :
            Drawable(other),
            m_type(other.m_type),
            m_path(other.m_path)
        {
            constructVAO();
        }

        __forceinline DrawPath(DrawPath&& other) :
            Drawable(std::move(other)),
            m_type(other.m_type),
            m_path(other.m_path),
            m_VAO(other.m_VAO),
            m_VBO(other.m_VBO)
        {
            other.m_VAO = 0;
            other.m_VBO = 0;
        }

        ~DrawPath()
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
        }

        __forceinline void updatePath(Path path)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            for (int i = 0; i < 5; i++)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3), glm::value_ptr(path.points[i]));
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void draw() override
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

            for (int i = 0; i < 4; i++)
            {
                glDrawArrays(GL_LINES, i, 2);
            }
            glBindVertexArray(0);

            m_texturePack->postDraw();
        }

    private:
        __forceinline void constructVAO()
        {
            glGenVertexArrays(1, &m_VAO);
            glGenBuffers(1, &m_VBO);

            glBindVertexArray(m_VAO);

            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 5, m_path.points, m_type);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        const GLenum m_type;

        Path m_path;

        GLuint m_VAO;
        GLuint m_VBO;
    };
}