#include "SBGravity.hpp"

ee::SBGravity::SBGravity() :
    m_acceleration(glm::vec3(0.f, -9.80665f, 0.f))
{
}

ee::SBGravity::SBGravity(glm::vec3 acceleration) :
    m_acceleration(acceleration)
{
}

void ee::SBGravity::applyForce(SBObject* io_object)
{
    io_object->m_resultantForce += io_object->m_mass * m_acceleration;
}

ee::SBGlobalForceGen* ee::SBGravity::getCopy() const
{
    return new SBGravity(*this);
}
