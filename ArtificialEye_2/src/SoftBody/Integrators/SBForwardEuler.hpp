#pragma once

#include "SBIntegrator.hpp"

namespace ee
{
    class SBForwardEuler : public SBIntegrator
    {
    public:
        SBForwardEuler(float constTimeStep);

        void integrate(glm::vec3 acceleration, SBObject* object) override;
    };
}