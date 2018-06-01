#pragma once

#include "KDTree.hpp"

namespace ee
{
    // Used to intersect a mesh:
    struct RTMeshIntersection
    {
        glm::vec3 point;
        glm::vec3 normal;
        int face;
    };

	class RTMesh
	{
	public:
        RTMesh(const Mesh* mesh, float refraction, bool reflection);

		bool intersect(Ray ray, int ignoreFace, RTMeshIntersection* o_int) const;

        float getRefraction() const;
        float getReflection() const;

	private:
        KDTree m_tree;

        float m_refraction;
        float m_reflection;
		const Mesh* const m_mesh;
	};
}