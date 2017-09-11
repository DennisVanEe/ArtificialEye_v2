#include "SBLengthConstraint.hpp"

#include <glm/gtx/norm.hpp>

ee::SBLengthConstraint::SBLengthConstraint(const float length, SBObject* const objA, SBObject* objB) :
    m_length(length),
    m_objA(objA),
    m_objB(objB)
{
}

void ee::SBLengthConstraint::satisfyConstraint()
{
    Vector3 direction = m_objB->m_currPosition - m_objA->m_currPosition;
    const float currLength = glm::length(direction);
    direction = glm::normalize(direction);

    if (direction != Vector3(0.f, 0.f, 0.f))
    {
        const Vector3 moveVec = 0.1f * (currLength - m_length) * direction;
        m_objA->m_currPosition += moveVec;
        m_objB->m_currPosition -= moveVec;
    }
}

ee::SBConstraint* ee::SBLengthConstraint::getCopy() const
{
    return new SBLengthConstraint(*this);
}
