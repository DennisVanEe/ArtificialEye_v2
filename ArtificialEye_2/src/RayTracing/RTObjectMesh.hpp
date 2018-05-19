#pragma once

#include "RTObject.hpp"

namespace ee
{
	class RTObjectMesh : public RTObject
	{
	public:
		RTObjectMesh(std::string name, Mesh* mesh, float refraction = std::numeric_limits<float>::quiet_NaN(), bool reflective = false);

		bool calcIntersection(Ray ray, int ignoreTriangle, bool towardsPhoto) const override;
		glm::vec3 intNormalInterpolated() const override;

		glm::vec3 intPoint() const override;
		glm::vec3 intNormalFace() const override;
		int intFace() const override;

		glm::mat4 getPosition() const override;

		const Mesh* getMesh() const;

		RTObject* getCopy() const override;

	private:
		mutable bool		m_intersected;
		mutable glm::vec3	m_cachedPoint;
		mutable int			m_cachedFace;

		const Mesh* const m_mesh;
	};
}