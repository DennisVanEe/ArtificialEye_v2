#pragma once

#include <unordered_map>
#include <list>
#include "Mesh.hpp"
#include "../RayTracing/RTObjectMesh.hpp"

namespace ee
{
	class MeshManager
	{
	public:
		MeshManager& initialize() const
		{
			static MeshManager meshManager;
			return meshManager;
		}

		// returns a copy, this could be useful
		RTObjectMesh addMesh(Mesh mesh, std::string name, float refraction = std::numeric_limits<float>::quiet_NaN(), bool reflective = false)
		{
			m_meshPool.push_front(mesh);
			RTObjectMesh rt(name, &m_meshPool.front(), refraction, reflective);
			m_rtobjectPool.insert(make_pair(name, rt));
			return rt;
		}

		RTObjectMesh getRTObjectMesh(const std::string& name) const
		{
			const auto it = m_rtobjectPool.find(name);
			if (it == m_rtobjectPool.end())
			{
				return RTObjectMesh("", nullptr);
			}
			return it->second;
		}

		Mesh* getMesh(const std::string& name)
		{
			return getRTObjectMesh(name).getMesh();
		}

		const Mesh* getMesh(const std::string& name) const
		{
			return getRTObjectMesh(name).getMesh();
		}

	private:
		MeshManager()
		{
		}

		std::unordered_map<std::string, RTObjectMesh> m_rtobjectPool;
		std::list<Mesh> m_meshPool;
	};
}