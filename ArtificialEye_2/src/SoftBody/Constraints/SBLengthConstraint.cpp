#include "SBLengthConstraint.hpp"

#include <glm/gtx/norm.hpp>

ee::SBLengthConstraint::SBLengthConstraint(const Float length, SBObject* const objA, SBObject* objB, Float factor, int id) :
    m_length(length),
    m_factor(factor),
    m_id(id),
    m_objA(objA),
    m_objB(objB)
{
}

void ee::SBLengthConstraint::satisfyConstraint()
{
    Vec3 direction = m_objB->m_currPosition - m_objA->m_currPosition;
    const Float currLength = glm::length(direction);
    direction = glm::normalize(direction);

    if (direction != Vec3())
    {
        Vec3 moveVec = m_factor * (currLength - m_length) * direction;
        m_objA->m_currPosition += moveVec;
        m_objB->m_currPosition -= moveVec;
    }
}