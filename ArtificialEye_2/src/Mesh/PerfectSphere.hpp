#pragma once

#include "../Types.hpp"

namespace ee
{
	class PerfectSphere
	{
	public:
		PerfectSphere() {} // A unit sphere (one with radius 1)

		

	private:
		Mat4 m_position;
		Mat4 m_invPosition;
	};
}