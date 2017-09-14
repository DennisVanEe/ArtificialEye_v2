#pragma once

#include "../../Types.hpp"

namespace ee
{
    // passive objects will act like the muscle in this case
    enum class SBObjectType { PASSIVE, ACTIVE };

    class SBObject
    {
    public:
        SBObject(float mass, SBObjectType type);

        void resetForces();

        virtual void update(float time) = 0;
        virtual SBObject* getCopy() const = 0;

    public:
        glm::vec3 m_currPosition;
        glm::vec3 m_prevPosition;
        glm::vec3 m_currVelocity;
        glm::vec3 m_resultantForce;
        float m_mass;

        SBObjectType m_type;
    };
}