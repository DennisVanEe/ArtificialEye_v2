#include "SBObject.hpp"

ee::SBObject::SBObject(float mass, SBObjectType type) :
    m_type(type),
    m_mass(mass)
{
}

void ee::SBObject::resetForces()
{
    m_resultantForce = glm::dvec3();
}