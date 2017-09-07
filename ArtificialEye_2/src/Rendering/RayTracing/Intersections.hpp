#pragma once

#include <numeric>

#include "Ray.hpp"
#include "../Modeling/Mesh.hpp"

namespace ee
{
    std::pair<bool, Vector3> intersectTriangle(Ray ray, Vector3 p0, Vector3 p1, Vector3 p2);
    std::pair<std::size_t, Vector3> nearestIntersection(const Mesh* mesh, Ray ray, std::size_t ignore = std::numeric_limits<std::size_t>::max());

    Ray meshRefract(const Mesh* mesh, Ray ray);
}