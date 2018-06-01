#include "Bound.hpp"

#include <glm/common.hpp>
#include <limits>

ee::Bound3::Bound3() :
    pmin(std::numeric_limits<float>::max()),
    pmax(std::numeric_limits<float>::min())
{
}

ee::Bound3::Bound3(glm::vec3 p) :
    pmin(p),
    pmax(p)
{
}

ee::Bound3::Bound3(glm::vec3 p0, glm::vec3 p1) :
    pmin(glm::min(p0, p1)),
    pmax(glm::max(p0, p1))
{
}

bool ee::Bound3::intersect(Ray ray) const
{
    const glm::vec3 rayDirInv = glm::vec3(1.f) / ray.dir;

    float t1 = (pmin[0] - ray.origin[0]) * rayDirInv[0];
    float t2 = (pmax[0] - ray.origin[0]) * rayDirInv[0];

    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    for (int i = 1; i < 3; i++)
    {
        t1 = (pmin[i] - ray.origin[i]) * rayDirInv[i];
        t2 = (pmax[i] - ray.origin[i]) * rayDirInv[i];

        tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
        tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));
    }

    return tmax > std::max(tmin, 0.f);
}

int ee::Bound3::longestAxis() const
{
    const glm::vec3 diff = pmax - pmin;
    int result = 0;
    if (diff[result] < diff.y)
    {
        result = 1;
    }
    if (diff[result] < diff.z)
    {
        result = 2;
    }
    return result;
}

ee::Bound3 ee::combine(const Bound3& b0, const Bound3& b1)
{
    return Bound3(glm::min(b0.pmin, b1.pmin), glm::max(b0.pmax, b1.pmax));
}

ee::Bound3 ee::combine(const Bound3& b, const glm::vec3& p)
{
    return Bound3(glm::min(b.pmin, p), glm::max(b.pmax, p));
}

ee::Bound3 ee::boundTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    Bound3 result;
    result = combine(result, v0);
    result = combine(result, v1);
    result = combine(result, v2);
    return result;
}

ee::Bound3 ee::boundTriangle(glm::vec3 v[3])
{
    return boundTriangle(v[0], v[1], v[2]);
}

ee::Bound3 ee::boundTriangle(Triangle triangle)
{
    return boundTriangle(triangle[0], triangle[1], triangle[2]);
}
