#pragma once

#include "SBIntegrator.hpp"

namespace ee
{
    class SBForwardEuler : public SBIntegrator
    {
    public:
        SBForwardEuler(float constTimeStep);

        void integrate(Vector3 acceleration, SBObject* object) override;
    };
}