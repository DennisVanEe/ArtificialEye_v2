#pragma once

#include "../Drawable.hpp"
#include "../Renderer.hpp"
#include "../TexturePacks/TexturePack.hpp"
#include "../../Types.hpp"
#include "../../Types.hpp"

#include <vector>
#include <string>
#include <memory>
#include <glad/glad.h>

namespace ee
{
    class DrawLine : public Drawable
    {
    public:
        DrawLine(std::string textPack, glm::vec3 p0, glm::vec3 p1, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);
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

        virtual void draw() override;

    private:
        void constructVAO();
        const GLenum m_type;

        glm::vec3 m_points[2];

        GLuint m_VAO;
        GLuint m_VBO;
    };
}