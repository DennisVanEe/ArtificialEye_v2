#include "SBIntegrator.hpp"

ee::SBIntegrator::SBIntegrator(const Float constTimeStep) :
    m_constTimeStep(constTimeStep)
{
}

ee::Float ee::SBIntegrator::getTimeStep() const
{
    return m_constTimeStep;
}