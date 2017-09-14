#pragma once

#include "../../Types.hpp"

namespace ee
{
    struct Ray
    {
        glm::vec3 m_origin;
        glm::vec3 m_dir;

        Ray() {}
        Ray(glm::vec3 origin, glm::vec3 dir) : m_origin(origin), m_dir(dir) {}
    };

    struct LineSegment
    {
        glm::vec3 m_start;
        glm::vec3 m_end;

        LineSegment() {}
        LineSegment(glm::vec3 start, glm::vec3 end) : m_start(start), m_end(end) {}
    };
}