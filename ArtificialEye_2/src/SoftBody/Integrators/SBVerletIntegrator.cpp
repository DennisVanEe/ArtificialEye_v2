#include "SBVerletIntegrator.hpp"

#include <glm/GTX/norm.hpp>

ee::SBVerletIntegrator::SBVerletIntegrator(float timeStep) :
    SBIntegrator(timeStep)
{
}

ee::SBVerletIntegrator::SBVerletIntegrator(float timeStep, float drag) :
    SBIntegrator(timeStep),
    m_drag(glm::clamp(drag, 0.f, 1.f))
{
}

float ee::SBVerletIntegrator::getDrag() const
{
    return m_drag;
}

void ee::SBVerletIntegrator::setDrag(const float drag)
{
    m_drag = glm::clamp(drag, 0.f, 1.f);
}

void ee::SBVerletIntegrator::integrate(const glm::vec3 acceleration, SBObject* const obj)
{
    glm::vec3 newPosition = (2.f - m_drag) * obj->m_currPosition -
        (1.f - m_drag) * obj->m_prevPosition + acceleration * m_constTimeStep * m_constTimeStep;

    newPosition = glm::length2(newPosition - obj->m_currPosition) < FLT_EPSILON ? obj->m_currPosition :
        newPosition;

    obj->m_prevPosition = obj->m_currPosition;
    obj->m_currPosition = newPosition;
}

ee::SBIntegrator* ee::SBVerletIntegrator::getCopy() const
{
    return new SBVerletIntegrator(*this);
}
