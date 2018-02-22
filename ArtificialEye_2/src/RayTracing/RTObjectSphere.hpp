#pragma once

#include <glm/matrix.hpp>
#include "RTObject.hpp"

namespace ee
{
	class RTObjectSphere : public RTObject
	{
	public:
		RTObjectSphere(std::string name, glm::mat4 posMat, float refraction, bool reflective = false) :
			RTObject(name, refraction, reflective), 
			m_position(posMat),
			m_invPosition(glm::inverse(m_position)) 
		{
		}

        bool calcIntersection(Ray ray, int ignoreTriangle) const override;

		virtual glm::vec3 intPoint() const 
		{ 
			return m_intPoint; 
		}

        virtual glm::vec3 intNormalInterpolated() const
		{ 
			return intNormalFace(); 
		}

		virtual glm::vec3 intNormalFace() const 
		{ 
			return m_intNormal; 
		}

		virtual int  intFace() const 
		{ 
			return -1; 
		}

		glm::mat4 getPosition() const override 
		{
			return m_position;
		}

		void setPosition(glm::mat4 pos) override
		{
			m_position = pos;
			m_invPosition = glm::inverse(m_position);
		}

		RTObject* getCopy() const override
		{ 
			return new RTObjectSphere(*this); 
		}

	private:
		glm::mat4 m_position;
		glm::mat4 m_invPosition;

		mutable glm::vec3 m_intPoint;
		mutable glm::vec3 m_intNormal;
	};
}