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
        Vector3 m_currPosition;
        Vector3 m_prevPosition;
        Vector3 m_currVelocity;
        Vector3 m_resultantForce;
        float m_mass;

        SBObjectType m_type;
    };
}