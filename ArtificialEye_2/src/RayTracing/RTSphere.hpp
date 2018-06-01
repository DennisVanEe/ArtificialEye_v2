#pragma once

#include <glm/matrix.hpp>
#include "../Types.hpp"
#include "../Mesh/Sphere.hpp"

namespace ee
{
    // Used to intersect a mesh:
    struct RTSphereIntersection
    {
        glm::vec3 point;
        glm::vec3 normal;
    };

	class RTSphere
	{
	public:
        RTSphere(const Sphere* sphere, float refraction, bool reflection);

        bool intersect(Ray ray, RTSphereIntersection* o_int) const;
        bool intersect(Ray ray) const;

        const Sphere* getSphere() const;

        float getRefraction() const;
        bool  isReflective() const;

	private:
        const Sphere* const m_sphere;

        float m_refraction;
        float m_reflection;
	};
}