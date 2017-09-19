#pragma once

#include <limits>
#include <utility>

#include "../Types.hpp"
#include "../Rendering/Modeling/Mesh.hpp"

namespace ee
{
    // returns whether or not there is an intersection and where (point)
    std::pair<bool, glm::vec3> intersectTriangle(Ray ray, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2);
    // returns the nearest intersection (point) and which triangle in the mesh (if any)
    std::pair<std::size_t, glm::vec3> nearestIntersectionMesh(const Mesh* mesh, Ray ray, std::size_t ignore = ULONG_MAX);

    namespace cust
    {
        // custom refract function (glm's acts weirdly)
        glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, float eta);
    }
}