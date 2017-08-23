#include "SBVerletIntegrator.hpp"

ee::SBVerletIntegrator::SBVerletIntegrator(Float timeStep) :
    SBIntegrator(timeStep)
{
}

ee::SBVerletIntegrator::SBVerletIntegrator(Float timeStep, Float drag) :
    SBIntegrator(timeStep),
    m_drag(glm::clamp(drag, F(0), F(1)))
{
}

ee::Float ee::SBVerletIntegrator::getDrag() const
{
    return m_drag;
}

void ee::SBVerletIntegrator::setDrag(const Float drag)
{
    m_drag = glm::clamp(drag, F(0), F(1));
}

void ee::SBVerletIntegrator::integrate(const Vector3 acceleration, SBObject* const obj)
{
    const Vector3 newPosition = (F(2) - m_drag) * obj->m_currPosition -
        (F(1) - m_drag) * obj->m_prevPosition + acceleration * m_constTimeStep * m_constTimeStep;

    obj->m_prevPosition = obj->m_currPosition;
    obj->m_currPosition = newPosition;
}

ee::SBIntegrator* ee::SBVerletIntegrator::getCopy() const
{
    return new SBVerletIntegrator(*this);
}
