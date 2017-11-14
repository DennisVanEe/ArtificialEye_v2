#pragma once

#include "SBIntegrator.hpp"

namespace ee
{
    class SBForwardEuler : public SBIntegrator
    {
    public:
        SBForwardEuler(Float constTimeStep) : SBIntegrator(constTimeStep) {}

        void integrate(Vec3 acceleration, SBObject* object) override
        {
            // x(t + dt) = x(t) + v(t)dt
            object->m_currPosition += object->m_currVelocity * m_constTimeStep;
            // v(t + dt) = v(t) + a(t)dt
            object->m_currVelocity += acceleration * m_constTimeStep;
        }
    };
}