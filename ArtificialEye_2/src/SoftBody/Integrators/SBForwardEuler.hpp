#pragma once

#include "SBIntegrator.hpp"

namespace ee
{
    class SBForwardEuler : public SBIntegrator
    {
    public:
        SBForwardEuler(float constTimeStep);

        void integrate(Vec3 acceleration, SBObject* object) override;
    };
}