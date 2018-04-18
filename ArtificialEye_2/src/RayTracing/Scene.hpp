#pragma once

#include <vector>
#include <memory>
#include "RTObject.hpp"
#include "RTRay.hpp"
#include "../Rendering/Drawable.hpp"

namespace ee
{
	class Scene
	{
	public:
        struct Object
        {
            std::unique_ptr<RTObject> rtObject;
            std::unique_ptr<Drawable> drawable;
        };

		// Can't decrease the size of an object
		void addObject(const RTObject* object, const Drawable* drawable)
		{
			RTObject* const newObject = object->getCopy();
            Drawable* const newDrawable = drawable->getCopy();

            Object tempObject;
            tempObject.rtObject = std::unique_ptr<RTObject>(newObject);
            tempObject.drawable = std::unique_ptr<Drawable>(newDrawable);
			m_objects.push_back(tempObject);
		}

		int getNumObjects() const
		{
			return static_cast<int>(m_objects.size());
		}

		const Object* getObject(int i) const
		{
			return &m_objects[i];
		}

        Object* getObject(int i)
		{
			return &m_objects[i];
		}

	private:
		std::vector<Object> m_objects;
	};
}