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
        SBSpring(float stiffness, float dampening, SBObject* objA, SBObject* objB);
        SBSpring(float stiffness, float dampening, float length, SBObject* objA, SBObject* objB);

        void applySpringForce();

    public:
        float m_stiffness;
        float m_dampening;
        float m_restLength;

    private:
        SBObject* m_objectA;
        SBObject* m_objectB;
    };
}