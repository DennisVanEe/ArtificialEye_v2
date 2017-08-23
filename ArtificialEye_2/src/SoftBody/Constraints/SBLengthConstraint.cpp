#include "SBLengthConstraint.hpp"

#include <glm/gtx/norm.hpp>

ee::SBLengthConstraint::SBLengthConstraint(const Float length, SBObject* const objA, SBObject* objB) :
    m_length(length),
    m_objA(objA),
    m_objB(objB)
{
}

void ee::SBLengthConstraint::satisfyConstraint()
{
    Vector3 direction = m_objB->m_currPosition - m_objA->m_currPosition;
    const Float currLength = glm::length(direction);
    direction = glm::normalize(direction);

    if (direction != Vector3(F(0), F(0), F(0)))
    {
        const Vector3 moveVec = F(0.5) * (currLength - m_length) * direction;
        m_objA->m_currPosition += moveVec;
        m_objB->m_currPosition -= moveVec;
    }
}

ee::SBConstraint* ee::SBLengthConstraint::getCopy() const
{
    return new SBLengthConstraint(*this);
}
