#pragma once

#include "../../Types.hpp"

namespace ee
{
    // passive objects will act like the muscle in this case
    enum class SBObjectType { PASSIVE, ACTIVE };

    class SBObject
    {
    public:
        SBObject(Float mass, SBObjectType type);

        void resetForces();

        virtual void update(Float time) = 0;
        virtual SBObject* getCopy() const = 0;

    public:
        Vector3 m_currPosition;
        Vector3 m_prevPosition;
        Vector3 m_currVelocity;
        Vector3 m_resultantForce;
        Float m_mass;

        SBObjectType m_type;
    };
}