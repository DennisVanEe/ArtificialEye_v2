#pragma once

#include <stack>
#include "../Types.hpp"

namespace ee
{
	class RTObject;

	struct RTRay : public Ray
	{
		RTRay() :
			Ray()
		{
		}

		RTRay(glm::vec3 o, glm::vec3 d) :
			Ray(o, d)
		{
		}

		// Keeps track of what objects the ray has entered and exited
		// This is a simplistic model, I am not expecting complex scenes
        // (with partial intersections and what not).
		// in this case, so this would work well enough.
		std::stack<const RTObject*> objects;
	};
}