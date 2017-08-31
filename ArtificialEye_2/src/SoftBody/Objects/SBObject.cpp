#include "SBObject.hpp"

ee::SBObject::SBObject(float mass, SBObjectType type) :
    m_currPosition(0.f, 0.f, 0.f),
    m_prevPosition(0.f, 0.f, 0.f),
    m_currVelocity(0.f, 0.f, 0.f),
    m_type(type),
    m_mass(mass)
{
}

void ee::SBObject::resetForces()
{
    m_resultantForce = Vector3(0.f, 0.f, 0.f);
}