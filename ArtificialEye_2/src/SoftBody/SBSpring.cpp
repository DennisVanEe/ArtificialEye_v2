#include "SBSpring.hpp"

ee::SBSpring::SBSpring(Float stiffness, Float dampening, SBObject* objA, SBObject* objB) :
    m_stiffness(stiffness),
    m_dampening(dampening),
    m_restLength(glm::length(objA->m_currPosition - objB->m_currPosition)),
    m_objectA(objA),
    m_objectB(objB)
{
}

ee::SBSpring::SBSpring(Float stiffness, Float dampening, Float length, SBObject* objA, SBObject* objB) :
    m_stiffness(stiffness),
    m_dampening(dampening),
    m_restLength(length),
    m_objectA(objA),
    m_objectB(objB)
{
}

void ee::SBSpring::applySpringForce()
{
    Vec3 direction = m_objectA->m_currPosition - m_objectB->m_currPosition;

    if (direction != Vec3())
    {
        Float currLength = glm::length(direction);
        direction = glm::normalize(direction);
        Vec3 force = -m_stiffness * ((currLength - m_restLength) * direction); // -kx * dir 
        force += -m_dampening * glm::dot(m_objectA->m_currVelocity - m_objectB->m_currVelocity, direction) * direction; // -yv (v is projected onto the "spring")

        force = zeroIfCloseVector(force); // helps maintain stability in the simulation

        m_objectA->m_resultantForce = m_objectA->m_resultantForce + force;
        m_objectB->m_resultantForce = m_objectB->m_resultantForce - force;
    }
}