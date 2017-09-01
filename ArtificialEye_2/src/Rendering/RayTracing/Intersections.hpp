#pragma once

#include "Ray.hpp"
#include "../Modeling/Mesh.hpp"

namespace ee
{
    float intersectTriangle(Ray ray, Vector3 p0, Vector3 p1, Vector3 p2);
}