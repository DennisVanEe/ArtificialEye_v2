#pragma once

#include <glm/matrix.hpp>
#include "../Types.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
	class EyeBall
	{
	public:
		EyeBall(glm::mat4 position, RTObjectMesh* lens) :
			lens(lens),
			position(position),
			invPosition(glm::inverse(position))
		{
		}

	public:
		const RTObjectMesh* lens;
		const glm::mat4 position;
		const glm::mat4 invPosition;
	};
}