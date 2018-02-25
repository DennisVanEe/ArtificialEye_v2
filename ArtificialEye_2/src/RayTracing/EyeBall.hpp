#pragma once

#include <glm/matrix.hpp>
#include "../Types.hpp"
#include "RTObjectMesh.hpp"
#include "RTObjectSphere.hpp"

namespace ee
{
	class EyeBall
	{
	public:
		EyeBall(glm::mat4 position, const RTObjectMesh* lens, const RTObjectSphere* eyeball) :
			lens(lens),
			eyeball(eyeball)
		{
		}

	public:
		const RTObjectMesh* lens;
		const RTObjectSphere* eyeball;
	};
}