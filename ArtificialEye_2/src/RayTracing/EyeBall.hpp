#pragma once

#include <glm/matrix.hpp>
#include "../Types.hpp"
#include "../Mesh/Mesh.hpp"

namespace ee
{
	class EyeBall
	{
	public:
		EyeBall(glm::mat4 position, Mesh* lens) :
			m_lens(lens),
			m_position(position),
			m_invPosition(glm::inverse(m_position))
		{
		}

	public:
		const Mesh* m_lens;
		const glm::mat4 m_position;
		const glm::mat4 m_invPosition;
	};
}