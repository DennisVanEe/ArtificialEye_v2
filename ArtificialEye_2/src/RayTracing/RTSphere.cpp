#include "RTSphere.hpp"

template <class T> 
int sign(T val) 
{
	return (T(0) < val) - (val < T(0));
}

#undef max
#undef min

ee::RTSphere::RTSphere(const Sphere* sphere, float refraction, bool reflection) :
    m_sphere(sphere),
    m_refraction(refraction),
    m_reflection(reflection)
{
}

bool ee::RTSphere::intersect(Ray ray, RTSphereIntersection* o_int) const
{
    const glm::mat4 invPosition = m_sphere->getInvPosition();

    const float min_dist = 0.001f;
    // const float max_dist = std::numeric_limits<float>::max();

    const glm::vec3 inv_dir = glm::vec3(invPosition * glm::vec4(ray.dir, 0.f)); // the direction is the one that is provided
    const glm::vec3 inv_origin = glm::vec3(invPosition * glm::vec4(ray.origin, 1.f)); // the origin is at the position of the sphere

    const float A = dot(inv_dir, inv_dir);
    const float B = dot(inv_origin, inv_dir);
    const float C = dot(inv_origin, inv_origin) - 1; // unit sphere

    const float discriminant = B * B - A * C;

    // check for the number of solutions:
    if (discriminant < 0)
    {
        return false; // std::make_pair(-1, Vec3());
    }

    float t_result;
    // two solutions:
    // --------------------------
    if (discriminant > 0)
    {
        const float sqrt_discr = std::sqrt(discriminant);

        // from Physically Based Rendering: Theory to Implementation 2nd Ed.
        const float q = -B - sign(B) * sqrt_discr;

        const float t_h1 = q / A;
        const float t_h2 = C / q;

        t_result = glm::min(t_h1, t_h2);
        if (t_result < min_dist) // if this one doesn't work, try the other one
        {
            t_result = glm::max(t_h1, t_h2);
        }
    }
    // one solution:
    // --------------------------
    else
    {
        t_result = -B / A;
    }

    // check result:
    // --------------------------
    if (t_result > min_dist) // && t_result < max_dist)
    {
        glm::vec4 normal = glm::vec4(inv_origin + (t_result * inv_dir), 0.f);

        normal = glm::transpose(invPosition) * normal;
        normal.w = 0;
        normal = normalize(normal); // normalize the normal

        o_int->normal = glm::vec3(normal);
        o_int->point = t_result * ray.dir + ray.origin;
        return true;
    }
    else
    {
        return false; // return std::make_pair(-1, Vec3());
    }
}

bool ee::RTSphere::intersect(Ray ray) const
{
    const glm::mat4 invPosition = m_sphere->getInvPosition();

    const float min_dist = 0.001f;
    // const float max_dist = std::numeric_limits<float>::max();

    const glm::vec3 inv_dir = glm::vec3(invPosition * glm::vec4(ray.dir, 0.f)); // the direction is the one that is provided
    const glm::vec3 inv_origin = glm::vec3(invPosition * glm::vec4(ray.origin, 1.f)); // the origin is at the position of the sphere

    const float A = dot(inv_dir, inv_dir);
    const float B = dot(inv_origin, inv_dir);
    const float C = dot(inv_origin, inv_origin) - 1; // unit sphere

    const float discriminant = B * B - A * C;

    // check for the number of solutions:
    if (discriminant < 0)
    {
        return false; // std::make_pair(-1, Vec3());
    }

    float t_result;
    // two solutions:
    // --------------------------
    if (discriminant > 0)
    {
        const float sqrt_discr = std::sqrt(discriminant);

        // from Physically Based Rendering: Theory to Implementation 2nd Ed.
        const float q = -B - sign(B) * sqrt_discr;

        const float t_h1 = q / A;
        const float t_h2 = C / q;

        t_result = glm::min(t_h1, t_h2);
        if (t_result < min_dist) // if this one doesn't work, try the other one
        {
            t_result = glm::max(t_h1, t_h2);
        }
    }
    // one solution:
    // --------------------------
    else
    {
        t_result = -B / A;
    }

    // check result:
    // --------------------------
    return t_result > min_dist;
}

const ee::Sphere* ee::RTSphere::getSphere() const
{
    return m_sphere;
}

float ee::RTSphere::getRefraction() const
{
    return m_refraction;
}

bool ee::RTSphere::isReflective() const
{
    return m_reflection;
}