#pragma once

#include "../Drawable.hpp"
#include "../Renderer.hpp"
#include "../TexturePacks/TexturePack.hpp"
#include "../../Types.hpp"
#include "../RayTracing/Ray.hpp"

#include <vector>
#include <string>
#include <memory>
#include <glad/glad.h>

namespace ee
{
    class Line : public Drawable
    {
    public:
        explicit Line(std::string textPack, glm::vec3 p0, glm::vec3 p1, int priority = 0, GLenum dataUsage = GL_STATIC_DRAW);

        void setPoint0(glm::vec3 p0);
        void setPoint1(glm::vec3 p1);

        void setRay(Ray ray, float length);
        void setLine(LineSegment line);

        glm::vec3 getPoint0() const { return m_points[0]; }
        glm::vec3 getPoint1() const { return m_points[1]; }

        Ray getRay() const
        {
            Ray result;
            result.m_dir = glm::normalize(m_points[1] - m_points[0]);
            result.m_origin = m_points[0];
            return result;
        }

        virtual void draw() override;

    private:
        glm::vec3 m_points[2];

        GLuint m_VAO;
        GLuint m_VBO;
    };
}