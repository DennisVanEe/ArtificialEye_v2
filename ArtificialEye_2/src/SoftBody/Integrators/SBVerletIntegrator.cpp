#include "SBVerletIntegrator.hpp"

#include <glm/GTX/norm.hpp>

void ee::SBVerletIntegrator::integrate(const Vec3 acceleration, SBObject* const obj)
{
    Vec3 newPosition = (2.0 - m_drag) * obj->m_currPosition -
        (1.0 - m_drag) * obj->m_prevPosition + acceleration * m_constTimeStep * m_constTimeStep;

    newPosition = glm::length2(newPosition - obj->m_currPosition) < glm::epsilon<Float>() ? obj->m_currPosition :
        newPosition;

    obj->m_prevPosition = obj->m_currPosition;
    obj->m_currPosition = newPosition;
}
