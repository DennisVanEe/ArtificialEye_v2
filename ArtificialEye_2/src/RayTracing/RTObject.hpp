#pragma once

#include <string>
#include "../Mesh/Mesh.hpp"
#include "../Types.hpp"

namespace ee
{
	// Object that can be intersected with
	class RTObject
	{
	public:
		RTObject(std::string name, float refraction, bool reflective = false) :
			m_name(name),
			isReflective(reflective),
			refracIndex(refraction)
		{
		}

		const std::string& getName() const
		{
			return m_name;
		}

		virtual bool calcIntersection(Ray ray, int ignoreTriangle) const = 0;
		virtual glm::vec3 intPoint() const = 0;
		virtual  glm::vec3 intNormalInterpolated() const = 0;
		virtual  glm::vec3 intNormalFace() const = 0;
		virtual int  intFace() const = 0;

		virtual glm::mat4 getPosition() const = 0;

		virtual RTObject* getCopy() const = 0;

		// virtual UDP getUDP() const = 0;

	public:
		bool isReflective;
		float refracIndex; // internal of object

	private:
		std::string m_name;
	};
}