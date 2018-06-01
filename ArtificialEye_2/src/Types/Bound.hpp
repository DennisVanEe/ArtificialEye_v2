#pragma once

#include <glm/vec3.hpp>
#include "../Mesh/Mesh.hpp"

namespace ee
{
    class Bound3
    {
    public:
        Bound3();
        Bound3(glm::vec3 p);
        Bound3(glm::vec3 p0, glm::vec3 p1);

        bool intersect(Ray ray) const;

        int longestAxis() const;

    public:
        glm::vec3 pmin, pmax;
    };

    Bound3 combine(const Bound3& b0, const Bound3& b1);
    Bound3 combine(const Bound3& b, const glm::vec3& p);
    
    Bound3 boundTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
    Bound3 boundTriangle(glm::vec3 v[3]);
    Bound3 boundTriangle(Triangle triangle);
}