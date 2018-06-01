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
    const glm::vec3 min = glm::min(v0, glm::min(v1, v2));
    const glm::vec3 max = glm::max(v0, glm::max(v1, v2));
    return Bound3(min, max);
}

ee::Bound3 ee::boundTriangle(glm::vec3 v[3])
{
    return boundTriangle(v[0], v[1], v[2]);
}

ee::Bound3 ee::boundTriangle(Triangle triangle)
{
    return boundTriangle(triangle[0], triangle[1], triangle[2]);
}
