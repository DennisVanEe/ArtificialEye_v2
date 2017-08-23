#include "SBGravity.hpp"

ee::SBGravity::SBGravity() :
    m_acceleration(Vector3(F(0), F(-9.80665), F(0)))
{
}

ee::SBGravity::SBGravity(Vector3 acceleration) :
    m_acceleration(acceleration)
{
}

void ee::SBGravity::applyForce(SBObject* io_object)
{
    io_object->m_resultantForce += io_object->m_mass * m_acceleration;
}

ee::SBForceGen* ee::SBGravity::getCopy() const
{
    return new SBGravity(*this);
}
