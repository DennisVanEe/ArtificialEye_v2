#pragma once

#include "Objects/SBObject.hpp"
#include "../Types.hpp"

#include <glm/gtx/norm.hpp>

namespace ee
{
    // no longer a force generator, it's its own thing now
    class SBSpring
    {
    public:
        SBSpring(Float stiffness, Float dampening, SBObject* objA, SBObject* objB);
        SBSpring(Float stiffness, Float dampening, Float length, SBObject* objA, SBObject* objB);

        void applySpringForce();

    public:
        Float m_stiffness;
        Float m_dampening;
        Float m_restLength;

    private:
        SBObject* m_objectA;
        SBObject* m_objectB;
    };
}