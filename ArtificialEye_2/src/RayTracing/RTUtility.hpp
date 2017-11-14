#pragma once

#include <limits>
#include <utility>

#include "../Types.hpp"
#include "../Rendering/Modeling/Mesh.hpp"

namespace ee
{
    // returns whether or not there is an intersection and where (point)
    std::pair<bool, Vec3> intersectTriangle(Ray ray, Vec3 p0, Vec3 p1, Vec3 p2);
    // returns the nearest intersection (point) and which triangle in the mesh (if any)
    std::pair<std::size_t, Vec3> nearestIntersectionMesh(const Mesh* mesh, Ray ray, std::size_t ignore = ULONG_MAX);

    void baryCentric(Vec3 p, Vec3 a, Vec3 b, Vec3 c, Float &u, Float &v, Float &w);

    namespace cust
    {
        // custom refract function (glm's acts weirdly)
        Vec3 refract(const Vec3& I, const Vec3& N, Float eta);
    }
}