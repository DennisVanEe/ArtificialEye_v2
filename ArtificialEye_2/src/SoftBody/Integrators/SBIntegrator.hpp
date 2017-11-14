#pragma once

#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBIntegrator
    {
    public:
        SBIntegrator(Float constTimeStep) : m_constTimeStep(constTimeStep) {}
        Float getTimeStep() const { return m_constTimeStep; }

        virtual void integrate(Vec3 acceleration, SBObject* object) = 0;

        virtual SBIntegrator* getCopy() const = 0;

    protected:
        const Float m_constTimeStep;
    };
}