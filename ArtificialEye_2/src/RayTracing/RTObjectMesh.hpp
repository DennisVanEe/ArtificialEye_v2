#pragma once

#include "RTObject.hpp"
#include "KDTree.hpp"

namespace ee
{
	class RTSphereMesh
	{
	public:
        RTSphereMesh(Mesh* mesh, float refraction, bool reflective);

		bool calcIntersection(Ray ray, int ignoreTriangle, IntersectionInfo* res) const;

		glm::mat4 getPosition() const;

		const Mesh* getMesh() const;

		RTObject* getCopy() const;

	private:


		const Mesh* const m_mesh;
	};
}