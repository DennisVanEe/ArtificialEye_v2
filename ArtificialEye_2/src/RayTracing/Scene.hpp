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
		Scene(int maxIteration = 100) : 
			m_maxIteration(maxIteration) 
		{
            assert(maxIteration > 0);
		}

		// Can't decrease the size of an object
		void addObject(const RTObject* object)
		{
			RTObject* newObject = object->getCopy();
			m_objects.push_back(std::unique_ptr<RTObject>(newObject));
		}

		size_t getNumObjects() const 
		{
			return static_cast<size_t>(m_objects.size());
		}

		const RTObject* getObject(size_t i) const
		{
			return m_objects[i].get(); 
		}

		RTObject* getObject(size_t i)
		{
			return m_objects[i].get();
		}

	private:
		int m_maxIteration;
		std::vector<std::unique_ptr<RTObject>> m_objects;
	};
}