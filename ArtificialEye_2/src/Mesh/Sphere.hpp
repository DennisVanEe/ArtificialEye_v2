#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace ee
{
    class Sphere
    {
    public:
        Sphere();
        Sphere(glm::mat4 pos);

        glm::mat4 getPosition() const;
        glm::mat4 getInvPosition() const;

        void setPosition(glm::mat4 pos);

    private:
        glm::mat4 m_position;
        glm::mat4 m_invPosition;
    };
}