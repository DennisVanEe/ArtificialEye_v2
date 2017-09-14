#include "SBLengthConstraint.hpp"

#include <glm/gtx/norm.hpp>

ee::SBLengthConstraint::SBLengthConstraint(const float length, SBObject* const objA, SBObject* objB, float factor) :
    m_length(length),
    m_factor(factor),
    m_objA(objA),
    m_objB(objB)
{
}

void ee::SBLengthConstraint::satisfyConstraint()
{
    glm::vec3 direction = m_objB->m_currPosition - m_objA->m_currPosition;
    const float currLength = glm::length(direction);
    direction = glm::normalize(direction);

    if (direction != glm::vec3())
    {
        glm::vec3 moveVec = m_factor * (currLength - m_length) * direction;
        m_objA->m_currPosition += moveVec;
        m_objB->m_currPosition -= moveVec;
    }
}

ee::SBConstraint* ee::SBLengthConstraint::getCopy() const
{
    return new SBLengthConstraint(*this);
}
