#include "SBMedium.hpp"

ee::SBMedium::SBMedium(Float dragCoef) :
    m_dragCoef(dragCoef)
{
}

void ee::SBMedium::applyForce(SBObject* object)
{
    object->m_resultantForce -= m_dragCoef * object->m_currVelocity;
}

ee::SBForceGen* ee::SBMedium::getCopy() const
{
    return new SBMedium(*this);
}
