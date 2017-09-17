#pragma once

#include <limits>
#include <utility>

#include "../../Types.hpp"
#include "../Modeling/Mesh.hpp"

namespace ee
{
    std::pair<bool, glm::vec3> intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
    std::pair<std::size_t, glm::vec3> nearestIntersection(const Mesh* mesh, Ray ray, std::size_t ignore = ULONG_MAX);

    namespace cust
    {
        glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, float eta);
    }
}