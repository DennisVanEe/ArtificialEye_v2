#pragma once

#include <vector>
#include <memory>
#include "RTObject.hpp"
#include "RTRay.hpp"

namespace ee
{
	class Scene
	{
	public:
		// Can't decrease the size of an object
		void addObject(const RTObject* object)
		{
			RTObject* const newObject = object->getCopy();
			m_objects.push_back(std::unique_ptr<RTObject>(newObject));
		}

		int getNumObjects() const
		{
			return static_cast<int>(m_objects.size());
		}

		const RTObject* getObject(int i) const
		{
			return m_objects[i].get();
		}

		RTObject* getObject(int i)
		{
			return m_objects[i].get();
		}

	private:
		std::vector<std::unique_ptr<RTObject>> m_objects;
	};
}