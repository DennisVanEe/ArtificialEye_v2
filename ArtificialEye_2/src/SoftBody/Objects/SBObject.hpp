#pragma once

#include "../../Types.hpp"

namespace ee
{
    // passive objects will act like the muscle in this case
    enum class SBObjectType { PASSIVE, ACTIVE };

    class SBObject
    {
    public:
        SBObject(Float mass, SBObjectType type) :
            m_type(type),
            m_mass(mass) {}

        void resetForces() { m_resultantForce = Vec3(); }

        virtual void update(Float time) = 0;
        virtual SBObject* getCopy() const = 0;

    public:
        Vec3 m_currPosition;
        Vec3 m_prevPosition;
        Vec3 m_currVelocity;
        Vec3 m_resultantForce;
        Float m_mass;

        SBObjectType m_type;
    };
}