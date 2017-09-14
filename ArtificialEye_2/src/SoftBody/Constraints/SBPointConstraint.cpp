#include "SBPointConstraint.hpp"

ee::SBPointConstraint::SBPointConstraint(const glm::vec3 point, SBObject* const obj) :
    m_point(point),
    m_obj(obj)
{
}

void ee::SBPointConstraint::satisfyConstraint()
{
    m_obj->m_currPosition = m_point;
}

ee::SBConstraint* ee::SBPointConstraint::getCopy() const
{
    return new SBPointConstraint(*this);
}
