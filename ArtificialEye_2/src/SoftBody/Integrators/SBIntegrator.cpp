#include "SBIntegrator.hpp"

ee::SBIntegrator::SBIntegrator(const float constTimeStep) :
    m_constTimeStep(constTimeStep)
{
}

float ee::SBIntegrator::getTimeStep() const
{
    return m_constTimeStep;
}