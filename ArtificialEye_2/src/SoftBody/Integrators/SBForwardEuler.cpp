#include "SBForwardEuler.hpp"

ee::SBForwardEuler::SBForwardEuler(const Float constTimeStep) :
    SBIntegrator(constTimeStep)
{
}

void ee::SBForwardEuler::integrate(const Vector3 acceleration, SBObject* object)
{
    // x(t + dt) = x(t) + v(t)dt
    object->m_currPosition += object->m_currVelocity * m_constTimeStep;
    // v(t + dt) = v(t) + a(t)dt
    object->m_currVelocity += acceleration * m_constTimeStep;
}