#include "Intersections.hpp"

#include <glm/glm.hpp>

const float ZERO = 1e-8;

float ee::intersectTriangle(Ray ray, Vector3 p0, Vector3 p1, Vector3 p2)
{
    Vector3 e0 = p1 - p0;
    Vector3 e1 = p2 - p0;
    // Calculate planes normal vector
    Vector3 normal = glm::cross(ray.m_dir, e1);
    float det = glm::dot(e0, normal);

    // Ray is parallel to plane
    if (det < ZERO && det > -ZERO)
    {
        return 0;
    }

    float invDet = 1 / det;
    Vector3 tVec = ray.m_origin - p0;
    float u = glm::dot(tVec, normal) * invDet;
    if (u < 0 || u > 1) 
    {
        return 0;
    }

    Vector3 qVec = glm::cross(tVec, e0);
    float v = glm::dot(ray.m_dir, qVec) * invDet;
    if (v < 0 || u + v > 1) 
    {
        return 0;
    }
    return glm::dot(e1, qVec) * invDet;
}