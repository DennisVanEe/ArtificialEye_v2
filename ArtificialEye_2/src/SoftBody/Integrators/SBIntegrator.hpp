#pragma once

#include "../Objects/SBObject.hpp"
#include "../../Types.hpp"

namespace ee
{
    class SBIntegrator
    {
    public:
        SBIntegrator(Float constTimeStep);
        Float getTimeStep() const;

        virtual void integrate(Vector3 acceleration, SBObject* object) = 0;

        virtual SBIntegrator* getCopy() const = 0;

    protected:
        const Float m_constTimeStep;
    };
}