#include "RTObjectSphere.hpp"

template <class T> 
int sign(T val) 
{
	return (T(0) < val) - (val < T(0));
}

#undef max
#undef min

bool ee::RTObjectSphere::calcIntersection(Ray ray, int ignoreTriangle) const
{
	float min_dist = 0.1f;
	float max_dist = std::numeric_limits<float>::max();

	glm::vec3 inv_dir = glm::vec3(m_invPosition * glm::vec4(ray.dir, 0.f)); // the direction is the one that is provided
    glm::vec3 inv_origin = glm::vec3(m_invPosition * glm::vec4(ray.origin, 1.f)); // the origin is at the position of the sphere

	float A = dot(inv_dir, inv_dir);
    float B = dot(inv_origin, inv_dir);
    float C = dot(inv_origin, inv_origin) - 1; // unit sphere

    float discriminant = B * B - A * C;

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
		float sqrt_discr = std::sqrt(discriminant);

		// from Physically Based Rendering: Theory to Implementation 2nd Ed.
		float q = -B - sign(B) * sqrt_discr;

		float t_h1 = q / A;
        float t_h2 = C / q;

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
	if (t_result > min_dist && t_result < max_dist)
	{
		glm::vec4 normal = glm::vec4(inv_origin + (t_result * inv_dir), 0.f);

		normal = glm::transpose(m_invPosition) * normal;
		normal.w = 0;
		normal = normalize(normal); // normalize the normal

		m_intNormal = glm::vec3(normal);
		m_intPoint = t_result * ray.dir + ray.origin;
		return true;
	}
	else
	{
		return false; // return std::make_pair(-1, Vec3());
	}
}